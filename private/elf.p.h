#pragma once

#include <elf.h>
#include "arch/elf.p.h"

#if __SIZEOF_POINTER__ == 8
#define ELFCLASSNATIVE ELFCLASS64
#define ELF_WIDTH 64
#else
#error "Unsupported pointer size"
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ELFDATANATIVE ELFDATA2LSB
#else
#error "Unsupported byte order"
#endif

typedef Elf64_Ehdr elf_ehdr_t;
typedef Elf64_Sym elf_sym_t;
typedef Elf64_Rel elf_rel_t;
typedef Elf64_Rela elf_rela_t;
typedef Elf64_Phdr elf_phdr_t;
typedef Elf64_Dyn elf_dyn_t;

// this array is shorter than ei_ident; only the first sizeof(ei_native_ident) bytes are compared, the rest are ignored.
static const unsigned char elf_native_ident[] =
        {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASSNATIVE, ELFDATANATIVE, EV_CURRENT};

#define ELF_ST_BIND(i) ELF64_ST_BIND(i)
#define ELF_ST_TYPE(i) ELF64_ST_TYPE(i)

#define ELF_R_SYM(i) ELF64_R_SYM(i)
#define ELF_R_TYPE(i) ELF64_R_TYPE(i)

#define SELECT_RELOC3(arch, name) R_##arch##_##name
#define SELECT_RELOC2(arch, name) SELECT_RELOC3(arch, name)
#define SELECT_RELOC(name) SELECT_RELOC2(R_ARCH_NAME, name)
#define SELECT_WRELOC3(width, ...) SELECT_RELOC(__VA_ARGS__##width)
#define SELECT_WRELOC2(width, ...) SELECT_WRELOC3(width, ##__VA_ARGS__)
#define SELECT_WRELOC(...) SELECT_WRELOC2(ELF_WIDTH, ##__VA_ARGS__)

#define R_NONE SELECT_RELOC(NONE)
#define R_POINTER SELECT_RELOC(ELF_WIDTH)
#define R_COPY SELECT_RELOC(COPY)
#define R_GLOB_DAT SELECT_RELOC(GLOB_DAT)
#define R_JUMP_SLOT SELECT_RELOC(JUMP_SLOT)
#define R_RELATIVE SELECT_RELOC(RELATIVE)
#define R_DTPMOD SELECT_WRELOC(DTPMOD)
#define R_DTPOFF SELECT_WRELOC(DTPOFF)
#define R_TPOFF SELECT_WRELOC(TPOFF)
#define R_IRELATIVE SELECT_RELOC(IRELATIVE)
