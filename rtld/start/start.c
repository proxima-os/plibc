#include <elf.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    intptr_t slide;
    const void *strtab;
    const void *symtab;
    size_t syment;
    struct {
        const uint32_t *buckets;
        const uint32_t *chains;
        uint32_t nbuckets;
    } hash;
} image_info_t;

typedef struct {
    const void *base;
    size_t size;
    size_t entry_size;
} reloc_tbl_t;

typedef struct {
    uintptr_t *got;
    reloc_tbl_t relocs;
} reloc_ctx_t;

typedef enum {
    RELOC_INIT, // Perform the regular relocations
    RELOC_LATE, // Perform relocations that need regular relocations to be completed
    RELOC_BIND, // Bind function references (this is done last because ifunc might call into the target's code)
} reloc_phase_t;

extern const void rtld_self_lazy_thunk;
static image_info_t rtld;
static image_info_t vdso;
static reloc_tbl_t jmp_relocs;

static const Elf64_Sym *get_symbol(image_info_t *image, uint32_t index) {
    return image->symtab + index * image->syment;
}

static uint32_t elf_hash(const unsigned char *name) {
    uint32_t hash = 0;

    for (;;) {
        unsigned char c = *name++;
        if (c == 0) break;

        hash = (hash << 4) + c;

        uint32_t top = hash & 0xf0000000;
        if (top) {
            hash ^= top >> 24;
            hash &= ~top;
        }
    }

    return hash;
}

static const Elf64_Sym *find_symbol(image_info_t *image, const unsigned char *name) {
    uint32_t index = image->hash.buckets[elf_hash(name) % image->hash.nbuckets];

    while (index != STN_UNDEF) {
        const Elf64_Sym *sym = get_symbol(image, index);
        const unsigned char *cand_name = image->strtab + sym->st_name;
        const unsigned char *test_name = name;

        for (;;) {
            unsigned char c1 = *cand_name++;
            unsigned char c2 = *test_name++;

            if (c1 != c2) break;
            if (c1 == 0) return sym;
        }

        index = image->hash.chains[index];
    }

    return NULL;
}

static uintptr_t resolve_symbol(uint64_t info, const Elf64_Sym **out) {
    image_info_t *image = &rtld;
    const Elf64_Sym *sym = get_symbol(image, ELF64_R_SYM(info));

    if (sym->st_shndx == SHN_UNDEF) {
        sym = find_symbol(&vdso, image->strtab + sym->st_name);
        image = &vdso;
    }

    if (out) *out = sym;
    return image->slide + sym->st_value;
}

// lazy binding is required because some ifunc resolvers might call other ifuncs that haven't been resolved yet
uintptr_t rtld_self_lazy(size_t reloc_idx) {
    const Elf64_Rela *rel = jmp_relocs.base + reloc_idx * jmp_relocs.entry_size;

    const Elf64_Sym *sym;
    uintptr_t addr = resolve_symbol(rel->r_info, &sym);

    if (ELF64_ST_TYPE(sym->st_info) == STT_GNU_IFUNC) {
        addr = ((uintptr_t (*)(void))addr)();
    }

    return addr;
}

static void execute_reloc(const Elf64_Rela *rel, reloc_phase_t phase) {
    void *ptr = (void *)(rel->r_offset + rtld.slide);
    unsigned long type = ELF64_R_TYPE(rel->r_info);

    if (type == R_X86_64_COPY) {
        if (phase == RELOC_LATE) {
            const Elf64_Sym *sym;
            unsigned char *dst = ptr;
            const unsigned char *src = (const void *)resolve_symbol(rel->r_info, &sym);

            for (size_t i = 0; i < sym->st_size; i++) {
                *dst++ = *src++;
            }
        }

        return;
    } else if (phase != RELOC_INIT) {
        return;
    }

    switch (type) {
    case R_X86_64_NONE: break;
    case R_X86_64_64: *(uint64_t *)ptr = resolve_symbol(rel->r_info, NULL) + rel->r_addend; break;
    case R_X86_64_GLOB_DAT: *(uintptr_t *)ptr = resolve_symbol(rel->r_info, NULL); break;
    case R_X86_64_JUMP_SLOT:
    // use lazy binding; relocate the plt thunk pointer
    case R_X86_64_IRELATIVE: *(uintptr_t *)ptr += rtld.slide; break;
    case R_X86_64_RELATIVE: *(uintptr_t *)ptr = rel->r_addend + rtld.slide; break;
    default: __builtin_trap();
    }
}

static void execute_table(reloc_tbl_t *tbl, reloc_phase_t phase) {
    for (size_t offset = 0; offset < tbl->size; offset += tbl->entry_size) {
        execute_reloc(tbl->base + offset, phase);
    }
}

static void perform_relocs(reloc_ctx_t *ctx, reloc_phase_t phase) {
    execute_table(&ctx->relocs, phase);
    execute_table(&jmp_relocs, phase);
}

static void setup_vdso(image_info_t *image, const void *base) {
    image->slide = (uintptr_t)base; // assume vdso is linked to 0
    const Elf64_Ehdr *header = base;
    const Elf64_Dyn *dynamic = NULL;

    for (size_t i = 0; i < header->e_phnum; i++) {
        const Elf64_Phdr *segment = base + header->e_phoff + i * header->e_phentsize;

        if (segment->p_type == PT_DYNAMIC) {
            dynamic = base + segment->p_offset;
            // verify image->slide
            if (segment->p_vaddr + image->slide != (uintptr_t)dynamic) __builtin_trap();
            break;
        }
    }

    for (const Elf64_Dyn *cur = dynamic; cur->d_tag != DT_NULL; cur++) {
        switch (cur->d_tag) {
        case DT_HASH: {
            const uint32_t *hash = (const void *)(cur->d_un.d_ptr + image->slide);
            image->hash.nbuckets = hash[0];
            image->hash.buckets = &hash[2];
            image->hash.chains = &image->hash.buckets[image->hash.nbuckets];
            break;
        }
        case DT_STRTAB: image->strtab = (const void *)(cur->d_un.d_ptr + image->slide); break;
        case DT_SYMTAB: image->symtab = (const void *)(cur->d_un.d_ptr + image->slide); break;
        case DT_SYMENT: image->syment = cur->d_un.d_val; break;
        }
    }
}

__attribute__((used)) void rtld_relocate_self(void **stack, Elf64_Dyn *dynamic) {
    // find auxv
    stack += (size_t)stack[0] + 2; // +2 to skip over argc itself and the argv terminator
    while (stack[0]) stack++;
    Elf64_auxv_t *auxv = (void *)&stack[1]; // [1] to skip over envp terminator

    reloc_ctx_t ctx = {};

    while (auxv->a_type != AT_NULL) {
        switch (auxv->a_type) {
        case AT_BASE: rtld.slide = auxv->a_un.a_val; break;
        case AT_SYSINFO_EHDR: setup_vdso(&vdso, (const void *)auxv->a_un.a_val); break;
        }

        auxv++;
    }

    for (Elf64_Dyn *cur = dynamic; cur->d_tag != DT_NULL; cur++) {
        switch (cur->d_tag) {
        case DT_PLTRELSZ: jmp_relocs.size = cur->d_un.d_val; break;
        case DT_PLTGOT: ctx.got = (void *)(cur->d_un.d_ptr + rtld.slide); break;
        case DT_STRTAB: rtld.strtab = (const void *)(cur->d_un.d_ptr + rtld.slide); break;
        case DT_SYMTAB: rtld.symtab = (const void *)(cur->d_un.d_ptr + rtld.slide); break;
        case DT_SYMENT: rtld.syment = cur->d_un.d_val; break;
        case DT_RELA: ctx.relocs.base = (const void *)(cur->d_un.d_ptr + rtld.slide); break;
        case DT_RELASZ: ctx.relocs.size = cur->d_un.d_val; break;
        case DT_RELAENT: ctx.relocs.entry_size = cur->d_un.d_val; break;
        case DT_JMPREL: jmp_relocs.base = (const void *)(cur->d_un.d_ptr + rtld.slide); break;
        }
    }

    jmp_relocs.entry_size = sizeof(Elf64_Rela);

    if (ctx.got) {
        ctx.got[2] = (uintptr_t)&rtld_self_lazy_thunk;
    }

    perform_relocs(&ctx, RELOC_INIT);
    perform_relocs(&ctx, RELOC_LATE);
}
