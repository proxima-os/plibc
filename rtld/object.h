#pragma once

#include "arch/elf.p.h"
#include "elf.p.h"
#include <elf.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// internal flags
#define RTLD_INITIALIZED (1 << 29)
#define RTLD_ROOT_OBJECT (1 << 28)
#define RTLD_DIAGNOSTICS (1 << 27)
#define RTLD_PHDRS_HEAP (1 << 26)

typedef void (*initfn_t)(int, char **, char **);

typedef struct {
    const uint32_t *buckets;
    const uint32_t *chain;
    uint32_t num_buckets;
} hash_table_t;

typedef struct {
    const void *data;
    size_t entry_size;
} symbol_table_t;

typedef struct {
    const void *data;
    size_t entry_size;
    size_t size;
} relocation_table_t;

typedef struct {
    const initfn_t *functions;
    size_t size;
} init_array_t;

typedef struct {
    uint64_t fs;
    uint64_t id;
} file_id_t;

typedef struct object {
    struct object *prev;
    struct object *next;
    struct object *objects_prev;
    struct object *objects_next;
    struct object *fileids_prev;
    struct object *fileids_next;
    struct object *global_prev;
    struct object *global_next;
    struct object *search_next;
    size_t references;
    file_id_t id;
    uint64_t id_hash;
    char *path;
    size_t origin_len;
    uintptr_t base;
    const void *phdrs;
    size_t phdr_entry_size;
    size_t phdr_count;
    uintptr_t memory;
    size_t mem_size;
    const elf_dyn_t *dynamic;
    struct object **dependencies;
    size_t num_deps;
    relocation_table_t pltrel;
    void *pltgot;
    hash_table_t hash_table;
    const char *strtab;
    symbol_table_t symtab;
    relocation_table_t rela;
    initfn_t init;
    initfn_t fini;
    const char *soname;
    uint64_t hash;
    const char *rpath;
    relocation_table_t rel;
    init_array_t init_array;
    init_array_t fini_array;
    const char *runpath;
    init_array_t preinit_array;
    size_t unterminated_dependents;
    size_t search_id;
    const void *tls_init_image;
    size_t tls_init_image_size;
    size_t tls_size;
    size_t tls_align;
    size_t tls_module;
    uintptr_t tls_offset;
    bool have_file_id : 1;
    bool phdrs_heap;
    bool symbolic : 1;
    bool textrel : 1;
    bool bind_now : 1;
    bool resolved : 1;
    bool relocated : 1;
    bool plt_reloc : 1;
    bool bound : 1;
    bool initialized : 1;
    bool terminated : 1;
    bool global : 1;
} object_t;

typedef struct {
    object_t *head;
    object_t *tail;
} search_list_t;

typedef struct {
    char *runpath;
    char **rpath;
    size_t nrpath;
    size_t crpath;
} obj_init_ctx_t;

__attribute__((visibility("hidden"))) extern bool force_bind_now asm("__plibc_rtld_force_bind_now");
__attribute__((visibility("hidden"))) extern const char *ld_library_path asm("__plibc_rtld_ld_library_path");
__attribute__((visibility("hidden"))) extern bool secure asm("__plibc_rtld_secure");
__attribute__((visibility("hidden"))) extern const void *subject_phdrs asm("__plibc_rtld_subject_phdrs");
__attribute__((visibility("hidden"))) extern size_t subject_phent asm("__plibc_rtld_subject_phent");
__attribute__((visibility("hidden"))) extern size_t subject_phnum asm("__plibc_rtld_subject_phnum");
__attribute__((visibility("hidden"))) extern uintptr_t subject_entry asm("__plibc_rtld_subject_entry");
__attribute__((visibility("hidden"))) extern object_t *root_object asm("__plibc_rtld_root_object");
__attribute__((visibility("hidden"))) extern int rtld_argc asm("__plibc_rtld_argc");
__attribute__((visibility("hidden"))) extern char **rtld_argv asm("__plibc_rtld_argv");
__attribute__((visibility("hidden"))) extern char **rtld_envp asm("__plibc_rtld_envp");
__attribute__((visibility("hidden"))) extern void *rtld_tls_area asm("__plibc_rtld_tls_area");

void rtld_init_paths(void) asm("__plibc_rtld_init_paths");

// on success, this function takes ownership of fd.
// base is only used if it cannot be figured out from the phdrs.
object_t *create_object(
        int fd,
        file_id_t *id,
        uintptr_t base,
        const void *phdrs,
        size_t phent,
        size_t phnum,
        int flags,
        obj_init_ctx_t *ctx
) asm("__plibc_rtld_create_object");
bool obj_init_for_search(obj_init_ctx_t *ctx, object_t *src, char ***runpath, size_t *nrunpath) asm(
        "__plibc_rtld_obj_init_for_search"
);
bool obj_init_finalize(object_t *root, int flags) asm("__plibc_rtld_obj_init_finalize");

object_t *address_to_object(uintptr_t address) asm("__plibc_rtld_address_to_object");
object_t *resolve_object(
        const char *path,
        int flags,
        object_t *source,
        obj_init_ctx_t *ctx,
        char **runpath,
        size_t nrunpath
) asm("__plibc_rtld_resolve_object");
void *obj_resolve(object_t *src, const char *name) __asm__("__plibc_rtld_resolve_symbol");

object_t *get_tls_object(size_t module) asm("__plibc_rtld_get_tls_object");

void obj_ref(object_t *obj) asm("__plibc_rtld_obj_ref");
void obj_deref(object_t *obj) asm("__plibc_rtld_obj_deref");

static inline bool is_valid_image(const elf_ehdr_t *header) {
    return !memcmp(header->e_ident, elf_native_ident, sizeof(elf_native_ident)) && header->e_type == ET_DYN &&
           header->e_machine == EM_NATIVE && header->e_version == EV_CURRENT;
}

static inline void obj_init_cleanup(obj_init_ctx_t *ctx) {
    for (size_t i = 0; i < ctx->nrpath; i++) {
        free(ctx->rpath[i]);
    }

    free(ctx->rpath);
}
