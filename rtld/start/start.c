#include "compiler.h"
#include "rtld.h"
#include <elf.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/auxv.h>

/* NOTE: Almost everything in this file runs *BEFORE* relocations, imports, and libc constructors
 * are processed! This means the code must be carefully written so that it does not use relocations
 * or library functions, even implicitly (e.g. memcpy and friends). */

typedef struct {
    uint32_t nbucket;
    uint32_t nchain;
    uint32_t data[];
} sym_ht_t;

typedef struct {
    void *data;
    size_t entry_size;
    size_t size;
} rels_t;

typedef void (*initfn_t)(int, char **, char **);

typedef struct {
    initfn_t *fns;
    size_t count;
} iarr_t;

typedef struct {
    uintptr_t slide;
    Elf64_Dyn *dynamic;
    const char *name;
    const sym_ht_t *sym_ht;
    const char *strtab;
    const void *symtab;
    size_t syment;
    size_t nvisit;
    rels_t rel, jmprel;
    initfn_t init, fini;
    iarr_t piarr, iarr, farr;
    bool have_dep : 1;
    bool symbolic : 1;
    bool bind_now : 1;
} object_t;

#define MAX_OBJ 2

static object_t objects[MAX_OBJ];
static object_t *dgraph[MAX_OBJ];
static size_t ndgraph;
static bool force_bind_now;

#define PHASE_REGULAR 0
#define PHASE_COPY 1
#define PHASE_BIND 2
#define NPHASE 3

static int phase_mask = (1 << PHASE_REGULAR) | (1 << PHASE_COPY);

extern const void __plibc_rtld_start_lazy_thunk;

static void rtld_fatal(void) {
    __builtin_trap();
}

static bool do_visit(object_t *obj, bool (*handler)(object_t *, void *), void *ctx, size_t iter) {
    if (obj->nvisit > iter) return true;
    obj->nvisit = iter + 1;

    if (!handler(obj, ctx)) return false;

    if (obj->have_dep) {
        for (Elf64_Dyn *cur = obj->dynamic; cur->d_tag != DT_NULL; cur++) {
            if (cur->d_tag == DT_NEEDED) {
                // setup_from_dynamic transforms these into object indices
                if (!do_visit(&objects[cur->d_un.d_val], handler, ctx, iter)) return false;
            }
        }
    }

    return true;
}

static bool visit_deps(object_t *obj, bool (*handler)(object_t *, void *), void *ctx) {
    static size_t visits;
    return do_visit(obj, handler, ctx, visits++);
}

static int compare_string_prefix(const char *s1, const char *s2, size_t n) {
    while (n--) {
        unsigned char c1 = *s1++;
        unsigned char c2 = *s2++;

        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) break;
    }

    return 0;
}

static int compare_string(const char *s1, const char *s2) {
    return compare_string_prefix(s1, s2, -1);
}

static uint32_t elf_hash(const char *name) {
    uint32_t hash = 0;

    for (;;) {
        unsigned char c = *name++;
        if (!c) break;

        hash = (hash << 4) + c;

        uint32_t top = hash & 0xf0000000;

        if (top) {
            hash ^= top >> 24;
            hash &= ~top;
        }
    }

    return hash;
}

static const Elf64_Sym *get_symbol(object_t *obj, size_t idx) {
    return obj->symtab + idx * obj->syment;
}

static const Elf64_Sym *resolve_within(object_t *obj, const char *name) {
    if (!obj->sym_ht) return NULL;
    uint32_t hash = elf_hash(name);

    uint32_t idx = obj->sym_ht->data[hash % obj->sym_ht->nbucket];

    while (idx != STN_UNDEF) {
        const Elf64_Sym *candidate = get_symbol(obj, idx);

        if (candidate->st_shndx != STN_UNDEF || candidate->st_value != 0) {
            const char *candidate_name = obj->strtab + candidate->st_name;

            if (!compare_string(name, candidate_name)) return candidate;
        }

        idx = obj->sym_ht->data[obj->sym_ht->nbucket + idx];
    }

    return NULL;
}

static void resolve_symbol(object_t **obj, const Elf64_Sym **sym) {
    object_t *src = *obj;
    const Elf64_Sym *cur = *sym;
    bool have_weak = ELF64_ST_BIND(cur->st_info) == STB_WEAK;
    if (src->symbolic && !have_weak && (cur->st_shndx != SHN_UNDEF || cur->st_value != 0)) return;
    const char *name = src->strtab + cur->st_name;

    for (size_t i = 0; i < ndgraph; i++) {
        object_t *ocur = dgraph[i];
        const Elf64_Sym *scur = resolve_within(ocur, name);

        if (scur) {
            *obj = ocur;
            *sym = scur;

            if (ELF64_ST_BIND(scur->st_info) != STB_WEAK) return;
            have_weak = true;
        }
    }

    if (!have_weak) rtld_fatal(); // unresolved reference
}

static void *sym_to_ptr(object_t *obj, const Elf64_Sym *sym) {
    void *ptr = (void *)(obj->slide + sym->st_value);

    if (ELF64_ST_TYPE(sym->st_info) == STT_GNU_IFUNC) {
        ptr = ((void *(*)(void))ptr)();
    }

    return ptr;
}

USED void *__plibc_rtld_start_lazy(object_t *obj, size_t idx) {
    Elf64_Rela *rel = obj->jmprel.data + idx * obj->jmprel.entry_size;
    void **ptr = (void **)(rel->r_offset + obj->slide);

    const Elf64_Sym *sym = get_symbol(obj, ELF64_R_SYM(rel->r_info));
    resolve_symbol(&obj, &sym);
    void *addr = sym_to_ptr(obj, sym);

    *(void **)ptr = addr;
    return addr;
}

static void do_reloc(object_t *obj, Elf64_Rela *rel, int phase) {
    void *ptr = (void *)(rel->r_offset + obj->slide);
    uint32_t type = ELF64_R_TYPE(rel->r_info);

    if (type == R_X86_64_COPY) {
        if (phase == PHASE_COPY) {
            const Elf64_Sym *sym = get_symbol(obj, ELF64_R_SYM(rel->r_info));
            resolve_symbol(&obj, &sym);

            unsigned char *dst = ptr;
            const unsigned char *src = sym_to_ptr(obj, sym);

            for (size_t i = 0; i < sym->st_size; i++) {
                *dst++ = *src++;
            }
        }

        return;
    } else if (type == R_X86_64_JUMP_SLOT || type == R_X86_64_IRELATIVE) {
        if (phase == PHASE_REGULAR) {
            // relocate the plt thunk
            *(uintptr_t *)ptr += obj->slide;
        } else if (phase == PHASE_BIND) {
            // bind it
            const Elf64_Sym *sym = get_symbol(obj, ELF64_R_SYM(rel->r_info));
            resolve_symbol(&obj, &sym);
            *(void **)ptr = sym_to_ptr(obj, sym);
        }

        return;
    } else if (phase != PHASE_REGULAR) {
        return;
    }

    switch (ELF64_R_TYPE(rel->r_info)) {
    case R_X86_64_NONE: break;
    case R_X86_64_64: {
        const Elf64_Sym *sym = get_symbol(obj, ELF64_R_SYM(rel->r_info));
        resolve_symbol(&obj, &sym);
        *(uint64_t *)ptr = (uintptr_t)sym_to_ptr(obj, sym) + rel->r_addend;
        break;
    }
    case R_X86_64_GLOB_DAT: {
        const Elf64_Sym *sym = get_symbol(obj, ELF64_R_SYM(rel->r_info));
        resolve_symbol(&obj, &sym);
        *(uint64_t *)ptr = (uintptr_t)sym_to_ptr(obj, sym);
        break;
    }
    case R_X86_64_RELATIVE: *(uintptr_t *)ptr = rel->r_addend + obj->slide; break;
    default: rtld_fatal(); break; // unknown relocation type
    }
}

static void do_relocs(object_t *obj, rels_t *rels, int phase) {
    for (size_t i = 0; i < rels->size; i += rels->entry_size) {
        do_reloc(obj, rels->data + i, phase);
    }
}

static void reloc_obj(object_t *obj, int phase) {
    if (phase == PHASE_BIND) {
        if (force_bind_now || obj->bind_now) do_relocs(obj, &obj->jmprel, phase);
        return;
    }

    do_relocs(obj, &obj->rel, phase);
    do_relocs(obj, &obj->jmprel, phase);
}

static object_t *setup_from_dynamic(uintptr_t slide, Elf64_Dyn *dynamic) {
    static size_t num_objects;
    if (num_objects == MAX_OBJ) rtld_fatal(); // too many objects
    object_t *obj = &objects[num_objects++];

    obj->slide = slide;
    obj->dynamic = dynamic;
    obj->jmprel.entry_size = sizeof(Elf64_Rela);

    uintptr_t soname = 0;

    for (Elf64_Dyn *cur = dynamic; cur->d_tag != DT_NULL; cur++) {
        switch (cur->d_tag) {
        case DT_NEEDED: obj->have_dep = true; break;
        case DT_PLTRELSZ: obj->jmprel.size = cur->d_un.d_val; break;
        case DT_PLTGOT: {
            const void **got = (void *)(cur->d_un.d_ptr + slide);
            got[1] = obj;
            got[2] = &__plibc_rtld_start_lazy_thunk;
            break;
        }
        case DT_HASH: obj->sym_ht = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_STRTAB: obj->strtab = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_SYMTAB: obj->symtab = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_RELA: obj->rel.data = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_RELASZ: obj->rel.size = cur->d_un.d_val; break;
        case DT_RELAENT: obj->rel.entry_size = cur->d_un.d_val; break;
        case DT_SYMENT: obj->syment = cur->d_un.d_val; break;
        case DT_INIT: obj->init = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_FINI: obj->fini = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_SONAME: soname = cur->d_un.d_val; break;
        case DT_SYMBOLIC: obj->symbolic = true; break;
        case DT_REL:
        case DT_RELSZ:
        case DT_RELENT: rtld_fatal(); break; // x86_64 only uses RELA
        case DT_PLTREL:
            if (cur->d_un.d_val != DT_RELA) rtld_fatal(); // x86_64 only uses RELA
            break;
        // relocation requires remapping memory, which can't be done until after relocation
        case DT_TEXTREL: rtld_fatal(); break;
        case DT_JMPREL: obj->jmprel.data = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_BIND_NOW: obj->bind_now = true; break;
        case DT_INIT_ARRAY: obj->iarr.fns = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_FINI_ARRAY: obj->farr.fns = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_INIT_ARRAYSZ: obj->iarr.count = cur->d_un.d_val / sizeof(*obj->iarr.fns); break;
        case DT_FINI_ARRAYSZ: obj->farr.count = cur->d_un.d_val / sizeof(*obj->farr.fns); break;
        case DT_FLAGS:
            if (cur->d_un.d_val & DF_SYMBOLIC) obj->symbolic = true;
            if (cur->d_un.d_val & DF_TEXTREL) rtld_fatal(); // see comment for DT_TEXTREL
            if (cur->d_un.d_val & DF_BIND_NOW) obj->bind_now = true;
            break;
        case DT_PREINIT_ARRAY: obj->piarr.fns = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_PREINIT_ARRAYSZ: obj->piarr.count = cur->d_un.d_val / sizeof(*obj->piarr.fns); break;
        }
    }

    if (soname) obj->name = obj->strtab + soname;

    if (obj->have_dep) {
        for (Elf64_Dyn *cur = dynamic; cur->d_tag != DT_NULL; cur++) {
            if (cur->d_tag == DT_NEEDED) {
                const char *name = obj->strtab + cur->d_un.d_val;

                size_t i;
                for (i = 0; i < num_objects; i++) {
                    if (objects[i].name && !compare_string(objects[i].name, name)) break;
                }

                // can't resolve non-vdso dependencies during self-relocation
                if (i >= num_objects) rtld_fatal();

                cur->d_un.d_val = i;
            }
        }
    }

    return obj;
}

static void setup_vdso(uintptr_t addr) {
    Elf64_Ehdr *header = (Elf64_Ehdr *)addr;
    uintptr_t phdrs = addr + header->e_phoff;
    uintptr_t dynamic = 0;
    uintptr_t slide = addr; // if no other info available, assume linked to 0

    for (size_t i = 0; i < header->e_phnum; i++) {
        Elf64_Phdr *phdr = (Elf64_Phdr *)(phdrs + i * header->e_phentsize);

        switch (phdr->p_type) {
        case PT_DYNAMIC: dynamic = phdr->p_vaddr; break;
        case PT_PHDR: slide = phdrs - phdr->p_vaddr; break;
        }
    }

    setup_from_dynamic(slide, (Elf64_Dyn *)(dynamic + slide));
}

static bool build_dgraph_func(object_t *obj, UNUSED void *ctx) {
    dgraph[ndgraph++] = obj;
    if (obj->bind_now) phase_mask |= 1 << PHASE_BIND;
    return true;
}

static int argc;
static char **argv;
static char **envp;

static void do_preinitfns(object_t *obj) {
    for (size_t i = 0; i < obj->piarr.count; i++) {
        obj->piarr.fns[i](argc, argv, envp);
    }
}

static void do_initfns(object_t *obj) {
    if (obj->init) obj->init(argc, argv, envp);

    for (size_t i = 0; i < obj->iarr.count; i++) {
        obj->iarr.fns[i](argc, argv, envp);
    }
}

static void do_finifns(object_t *obj) {
    for (size_t i = obj->farr.count; i > 0; i--) {
        obj->farr.fns[i - 1](argc, argv, envp);
    }

    if (obj->fini) obj->fini(argc, argv, envp);
}

EXPORT void __plibc_rtld_init(uintptr_t *stack, Elf64_Dyn *dynamic, uintptr_t *got) {
    argc = stack[0];
    argv = (char **)&stack[1];
    stack = &stack[argc + 2]; // 1 for argc itself, 1 for the null terminator
    envp = (char **)stack;

    for (;;) {
        const char *str = (const char *)*stack++;
        if (!str) break;

        if (compare_string_prefix(str, "LD_BIND_NOW=", 12) && str[12] != 0) {
            force_bind_now = true;
            phase_mask |= 1 << PHASE_BIND;
        }
    }

    // set up vdso
    for (;;) {
        uintptr_t tag = stack[0];

        if (tag == AT_SYSINFO_EHDR) {
            setup_vdso(stack[1]);
        } else if (tag != AT_NULL) {
            stack += 2;
            continue;
        }

        break;
    }

    object_t *self_obj = setup_from_dynamic((uintptr_t)dynamic - got[0], dynamic);
    visit_deps(self_obj, build_dgraph_func, NULL);

    for (int i = 0; i < NPHASE; i++) {
        if (!(phase_mask & (1 << i))) continue;

        for (size_t j = 0; j < ndgraph; j++) {
            reloc_obj(dgraph[j], i);
        }
    }

    for (size_t i = ndgraph; i > 0; i--) {
        do_preinitfns(dgraph[i - 1]);
    }

    for (size_t i = ndgraph; i > 0; i--) {
        do_initfns(dgraph[i - 1]);
    }
}

EXPORT void __plibc_rtld_run_fini(void) {
    for (size_t i = 0; i < ndgraph; i++) {
        do_finifns(dgraph[i]);
    }
}
