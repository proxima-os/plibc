#include "start.h"
#include "arch/elf.p.h"
#include "compiler.h"
#include "elf.p.h"
#include "object.h"
#include "rtld.p.h"
#include <dlfcn.h>
#include <elf.h>
#include <fcntl.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/types.h>
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
    const void *data;
    size_t entry_size;
    size_t size;
} rels_t;

typedef void (*initfn_t)(int, char **, char **);

typedef struct {
    const initfn_t *fns;
    size_t count;
} iarr_t;

typedef struct {
    uintptr_t slide;
    const void *phdrs;
    size_t phdr_entry_size, phdr_count;
    const elf_dyn_t *dynamic;
    const char *name;
    const sym_ht_t *sym_ht;
    const char *strtab;
    const void *symtab;
    size_t syment;
    size_t nvisit;
    rels_t rel, rela, jmprel;
    initfn_t init, fini;
    iarr_t piarr, iarr, farr;
    bool have_dep : 1;
    bool symbolic : 1;
    bool bind_now : 1;
} start_object_t;

#define MAX_OBJ 2

static start_object_t objects[MAX_OBJ];
static start_object_t *dgraph[MAX_OBJ];
static size_t ndgraph;
static size_t num_objects;

#define PHASE_REGULAR 0
#define PHASE_COPY 1
#define PHASE_BIND 2
#define NPHASE 3

static int phase_mask = (1 << PHASE_REGULAR) | (1 << PHASE_COPY);

extern const void __plibc_rtld_start_lazy_thunk;

static void rtld_start_fatal(void) {
    __builtin_trap();
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

static bool do_visit(start_object_t *obj, bool (*handler)(start_object_t *, void *), void *ctx, size_t iter) {
    if (obj->nvisit > iter) return true;
    obj->nvisit = iter + 1;

    if (!handler(obj, ctx)) return false;

    if (obj->have_dep) {
        for (const elf_dyn_t *cur = obj->dynamic; cur->d_tag != DT_NULL; cur++) {
            if (cur->d_tag == DT_NEEDED) {
                const char *name = obj->strtab + cur->d_un.d_val;

                size_t i;
                for (i = 0; i < num_objects; i++) {
                    if (objects[i].name && !compare_string(objects[i].name, name)) {
                        if (!do_visit(&objects[i], handler, ctx, iter)) {
                            return false;
                        }

                        break;
                    }
                }

                // can't resolve non-vdso dependencies during self-relocation
                if (i >= num_objects) rtld_start_fatal();
            }
        }
    }

    return true;
}

static bool visit_deps(start_object_t *obj, bool (*handler)(start_object_t *, void *), void *ctx) {
    static size_t visits;
    return do_visit(obj, handler, ctx, visits++);
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

static const elf_sym_t *get_symbol(start_object_t *obj, size_t idx) {
    return obj->symtab + idx * obj->syment;
}

static const elf_sym_t *resolve_within(start_object_t *obj, const char *name) {
    if (!obj->sym_ht) return NULL;
    uint32_t hash = elf_hash(name);

    uint32_t idx = obj->sym_ht->data[hash % obj->sym_ht->nbucket];

    while (idx != STN_UNDEF) {
        const elf_sym_t *candidate = get_symbol(obj, idx);

        if (candidate->st_shndx != STN_UNDEF || candidate->st_value != 0) {
            const char *candidate_name = obj->strtab + candidate->st_name;

            if (!compare_string(name, candidate_name)) return candidate;
        }

        idx = obj->sym_ht->data[obj->sym_ht->nbucket + idx];
    }

    return NULL;
}

static void resolve_symbol(start_object_t **obj, const elf_sym_t **sym) {
    start_object_t *src = *obj;
    const elf_sym_t *cur = *sym;
    bool have_weak = ELF_ST_BIND(cur->st_info) == STB_WEAK;
    if (src->symbolic && !have_weak && (cur->st_shndx != SHN_UNDEF || cur->st_value != 0)) return;
    const char *name = src->strtab + cur->st_name;

    for (size_t i = 0; i < ndgraph; i++) {
        start_object_t *ocur = dgraph[i];
        const elf_sym_t *scur = resolve_within(ocur, name);

        if (scur) {
            *obj = ocur;
            *sym = scur;

            if (ELF_ST_BIND(scur->st_info) != STB_WEAK) return;
            have_weak = true;
        }
    }

    if (!have_weak) rtld_start_fatal(); // unresolved reference
}

static void *sym_to_ptr(start_object_t *obj, const elf_sym_t *sym) {
    void *ptr = (void *)(obj->slide + sym->st_value);

    if (ELF_ST_TYPE(sym->st_info) == STT_GNU_IFUNC) {
        ptr = ((void *(*)(void))ptr)();
    }

    return ptr;
}

USED void *__plibc_rtld_start_lazy(start_object_t *obj, size_t idx) {
    const elf_rela_t *rel = obj->jmprel.data + idx * obj->jmprel.entry_size;
    void **ptr = (void **)(rel->r_offset + obj->slide);

    const elf_sym_t *sym = get_symbol(obj, ELF_R_SYM(rel->r_info));
    resolve_symbol(&obj, &sym);
    void *addr = sym_to_ptr(obj, sym);

    *(void **)ptr = addr;
    return addr;
}

static void do_reloc(start_object_t *obj, const elf_rel_t *rel, int phase, bool rela) {
    void *ptr = (void *)(rel->r_offset + obj->slide);
    uint32_t type = ELF_R_TYPE(rel->r_info);

    if (type == R_COPY) {
        if (phase == PHASE_COPY) {
            const elf_sym_t *sym = get_symbol(obj, ELF_R_SYM(rel->r_info));
            resolve_symbol(&obj, &sym);

            unsigned char *dst = ptr;
            const unsigned char *src = sym_to_ptr(obj, sym);

            for (size_t i = 0; i < sym->st_size; i++) {
                *dst++ = *src++;
            }
        }

        return;
    } else if (type == R_JUMP_SLOT || type == R_IRELATIVE) {
        if (phase == PHASE_REGULAR) {
            // relocate the plt thunk
            *(uintptr_t *)ptr += obj->slide;
        } else if (phase == PHASE_BIND) {
            // bind it
            const elf_sym_t *sym = get_symbol(obj, ELF_R_SYM(rel->r_info));
            resolve_symbol(&obj, &sym);
            *(void **)ptr = sym_to_ptr(obj, sym);
        }

        return;
    } else if (phase != PHASE_REGULAR) {
        return;
    }

#define ADDEND(type) (rela ? (type)((const elf_rela_t *)rel)->r_addend : *(type *)ptr)

    switch (ELF_R_TYPE(rel->r_info)) {
    case R_NONE: break;
    case R_POINTER: {
        const elf_sym_t *sym = get_symbol(obj, ELF_R_SYM(rel->r_info));
        resolve_symbol(&obj, &sym);
        *(uintptr_t *)ptr = (uintptr_t)sym_to_ptr(obj, sym) + ADDEND(uintptr_t);
        break;
    }
    case R_GLOB_DAT: {
        const elf_sym_t *sym = get_symbol(obj, ELF_R_SYM(rel->r_info));
        resolve_symbol(&obj, &sym);
        *(uint64_t *)ptr = (uintptr_t)sym_to_ptr(obj, sym);
        break;
    }
    case R_RELATIVE: *(uintptr_t *)ptr = ADDEND(uintptr_t) + obj->slide; break;
    default: rtld_start_fatal(); break; // unknown relocation type
    }

#undef ADDEND
}

static void do_relocs(start_object_t *obj, rels_t *rels, int phase, bool rela) {
    for (size_t i = 0; i < rels->size; i += rels->entry_size) {
        do_reloc(obj, rels->data + i, phase, rela);
    }
}

static void reloc_obj(start_object_t *obj, int phase) {
    if (phase == PHASE_BIND) {
        if (force_bind_now || obj->bind_now) {
            do_relocs(obj, &obj->jmprel, phase, obj->jmprel.entry_size >= sizeof(elf_rela_t));
        }

        return;
    }

    do_relocs(obj, &obj->rel, phase, false);
    do_relocs(obj, &obj->rela, phase, true);
    do_relocs(obj, &obj->jmprel, phase, obj->jmprel.entry_size >= sizeof(elf_rela_t));
}

static start_object_t *setup_from_dynamic(
        uintptr_t slide,
        const elf_dyn_t *dynamic,
        const void *phdrs,
        size_t phdr_entry_size,
        size_t phdrs_count
) {
    if (num_objects == MAX_OBJ) rtld_start_fatal(); // too many objects
    start_object_t *obj = &objects[num_objects++];

    obj->slide = slide;
    obj->dynamic = dynamic;

    uintptr_t soname = 0;

    for (const elf_dyn_t *cur = dynamic; cur->d_tag != DT_NULL; cur++) {
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
        case DT_RELA: obj->rela.data = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_RELASZ: obj->rela.size = cur->d_un.d_val; break;
        case DT_RELAENT: obj->rela.entry_size = cur->d_un.d_val; break;
        case DT_SYMENT: obj->syment = cur->d_un.d_val; break;
        case DT_INIT: obj->init = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_FINI: obj->fini = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_SONAME: soname = cur->d_un.d_val; break;
        case DT_SYMBOLIC: obj->symbolic = true; break;
        case DT_REL: obj->rel.data = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_RELSZ: obj->rel.size = cur->d_un.d_val; break;
        case DT_RELENT: obj->rel.entry_size = cur->d_un.d_val; break;
        case DT_PLTREL:
            switch (cur->d_un.d_val) {
            case DT_RELA: obj->jmprel.entry_size = sizeof(elf_rela_t); break;
            case DT_REL: obj->jmprel.entry_size = sizeof(elf_rel_t); break;
            default: rtld_start_fatal();
            }
            break;
        // relocation requires remapping memory, which can't be done until after relocation
        case DT_TEXTREL: rtld_start_fatal(); break;
        case DT_JMPREL: obj->jmprel.data = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_BIND_NOW: obj->bind_now = true; break;
        case DT_INIT_ARRAY: obj->iarr.fns = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_FINI_ARRAY: obj->farr.fns = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_INIT_ARRAYSZ: obj->iarr.count = cur->d_un.d_val / sizeof(*obj->iarr.fns); break;
        case DT_FINI_ARRAYSZ: obj->farr.count = cur->d_un.d_val / sizeof(*obj->farr.fns); break;
        case DT_FLAGS:
            if (cur->d_un.d_val & DF_SYMBOLIC) obj->symbolic = true;
            if (cur->d_un.d_val & DF_TEXTREL) rtld_start_fatal(); // see comment for DT_TEXTREL
            if (cur->d_un.d_val & DF_BIND_NOW) obj->bind_now = true;
            break;
        case DT_PREINIT_ARRAY: obj->piarr.fns = (void *)(cur->d_un.d_ptr + slide); break;
        case DT_PREINIT_ARRAYSZ: obj->piarr.count = cur->d_un.d_val / sizeof(*obj->piarr.fns); break;
        }
    }

    if (soname) obj->name = obj->strtab + soname;

    obj->phdrs = phdrs;
    obj->phdr_count = phdrs_count;
    obj->phdr_entry_size = phdr_entry_size;

    return obj;
}

static int compare_memory(const void *s1, const void *s2, size_t n) {
    const unsigned char *b1 = s1;
    const unsigned char *b2 = s2;

    while (n--) {
        unsigned char c1 = *b1++;
        unsigned char c2 = *b2++;

        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
    }

    return 0;
}

static void setup_vdso(uintptr_t addr) {
    const elf_ehdr_t *header = (const elf_ehdr_t *)addr;

    if (compare_memory(header->e_ident, elf_native_ident, sizeof(elf_native_ident))) rtld_start_fatal();
    if (header->e_type != ET_DYN) rtld_start_fatal();
    if (header->e_machine != EM_NATIVE) rtld_start_fatal();
    if (header->e_version != EV_CURRENT) rtld_start_fatal();

    uintptr_t phdrs = addr + header->e_phoff;
    uintptr_t dynamic = 0;
    uintptr_t slide = addr; // if no other info available, assume linked to 0

    for (size_t i = 0; i < header->e_phnum; i++) {
        const elf_phdr_t *phdr = (const elf_phdr_t *)(phdrs + i * header->e_phentsize);

        switch (phdr->p_type) {
        case PT_DYNAMIC: dynamic = phdr->p_vaddr; break;
        case PT_PHDR: slide = phdrs - phdr->p_vaddr; break;
        }
    }

    setup_from_dynamic(
            slide,
            (const elf_dyn_t *)(dynamic + slide),
            (const void *)phdrs,
            header->e_phentsize,
            header->e_phnum
    );
}

static bool build_dgraph_func(start_object_t *obj, void *ctx) {
    dgraph[ndgraph++] = obj;
    if (obj->bind_now) phase_mask |= 1 << PHASE_BIND;
    return true;
}

static void do_preinitfns(start_object_t *obj) {
    for (size_t i = 0; i < obj->piarr.count; i++) {
        obj->piarr.fns[i](rtld_argc, rtld_argv, rtld_envp);
    }
}

static void do_initfns(start_object_t *obj) {
    if (obj->init) obj->init(rtld_argc, rtld_argv, rtld_envp);

    for (size_t i = 0; i < obj->iarr.count; i++) {
        obj->iarr.fns[i](rtld_argc, rtld_argv, rtld_envp);
    }
}

__attribute__((visibility("hidden"))) extern const elf_ehdr_t __ehdr_start;

EXPORT void __plibc_rtld_init(uintptr_t *stack, const elf_dyn_t *dynamic, uintptr_t *got) {
    rtld_argc = stack[0];
    rtld_argv = (char **)&stack[1];
    stack = &stack[rtld_argc + 2]; // 1 for argc itself, 1 for the null terminator
    rtld_envp = (char **)stack;

    for (;;) {
        const char *str = (const char *)*stack++;
        if (!str) break;

        if (compare_string_prefix(str, "LD_BIND_NOW=", 12) == 0 && str[12] != 0) {
            force_bind_now = true;
            phase_mask |= 1 << PHASE_BIND;
        } else if (compare_string_prefix(str, "LD_LIBRARY_PATH=", 16) == 0 && str[16] != 0) {
            ld_library_path = &str[16];
        }
    }

    // parse auxv
    for (;;) {
        uintptr_t tag = stack[0];
        if (tag == AT_NULL) break;

        switch (tag) {
        case AT_PHDR: subject_phdrs = (const void *)stack[1]; break;
        case AT_PHENT: subject_phent = stack[1]; break;
        case AT_PHNUM: subject_phnum = stack[1]; break;
        case AT_ENTRY: subject_entry = stack[1]; break;
        case AT_SECURE: secure = true; break;
        case AT_SYSINFO_EHDR: setup_vdso(stack[1]); break;
        }

        stack += 2;
    }

    start_object_t *self_obj = setup_from_dynamic(
            (uintptr_t)dynamic - got[0],
            dynamic,
            (const void *)&__ehdr_start + __ehdr_start.e_phoff,
            __ehdr_start.e_phentsize,
            __ehdr_start.e_phnum
    );
    visit_deps(self_obj, build_dgraph_func, NULL);

    for (int i = 0; i < NPHASE; i++) {
        if (!(phase_mask & (1 << i))) continue;

        for (size_t j = 0; j < ndgraph; j++) {
            reloc_obj(dgraph[j], i);
        }
    }

    asm("" ::: "memory");
    // syscalls are now available

    for (size_t i = ndgraph; i > 0; i--) {
        do_preinitfns(dgraph[i - 1]);
    }

    __plibc_init(rtld_argc, rtld_argv, rtld_envp);

    // libc functions are now available

    rtld_init_paths();

    for (size_t i = 0; i < num_objects; i++) {
        start_object_t *src = &objects[i];
        int flags = RTLD_GLOBAL | RTLD_LAZY | RTLD_INITIALIZED | RTLD_DIAGNOSTICS;
        int fd = HYDROGEN_INVALID_HANDLE;

        if (subject_phdrs == NULL && src == self_obj) {
            flags |= RTLD_ROOT_OBJECT;

            hydrogen_ret_t ret = hydrogen_fs_fopen(HYDROGEN_INVALID_HANDLE, O_CLOEXEC | O_CLOFORK);
            if (unlikely(ret.error)) panic("rtld: failed to open executable: %s\n", strerror(ret.error));
            fd = ret.integer;
        }

        obj_init_ctx_t ctx = {};
        object_t *obj = create_object(
                fd,
                NULL,
                src->slide,
                src->phdrs,
                src->phdr_entry_size,
                src->phdr_count,
                flags,
                &ctx
        );
        obj_init_cleanup(&ctx);

        if (unlikely(!obj) || unlikely(!obj_init_finalize(obj, flags))) {
            panic("rtld: failed to create initial object: %s\n", dlerror());
        }
    }

    for (size_t i = ndgraph; i > 0; i--) {
        do_initfns(dgraph[i - 1]);
    }
}
