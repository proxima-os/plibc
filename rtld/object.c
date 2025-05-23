#include "object.h"
#include "arch/elf.p.h"
#include "compiler.h"
#include "elf.p.h"
#include "rtld.h"
#include "rtld.p.h"
#include "stddef.h"
#include <ctype.h>
#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/memory.h>
#include <hydrogen/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool force_bind_now;
const char *ld_library_path;
bool secure;
const void *subject_phdrs;
size_t subject_phent;
size_t subject_phnum;
uintptr_t subject_entry;
object_t *root_object;
int rtld_argc;
char **rtld_argv;
char **rtld_envp;
void *rtld_tls_area;

static object_t **objects_table;
static size_t objects_capacity;
static size_t objects_count;

static object_t **fileids_table;
static size_t fileids_capacity;
static size_t fileids_count;

static object_t *object_list;

static object_t *global_head;
static object_t *global_tail;

static char **libpath;
static size_t nlibpath;
static size_t clibpath;

static size_t root_tls_size;
static size_t root_tls_align;
static size_t next_tls_module = 2;

static uint64_t hash_blob(const void *name, size_t len) {
    uint64_t hash = 0xcbf29ce484222325;

    while (len--) {
        hash *= 0x00000100000001b3;
        hash ^= *(const unsigned char *)name++;
    }

    return hash;
}

static object_t *get_object(const char *name) {
    if (!objects_count) return NULL;

    uint64_t hash = hash_blob(name, strlen(name));
    object_t *cur = objects_table[hash % objects_capacity];

    for (;;) {
        if (!cur || (cur->hash == hash && !strcmp(cur->soname, name))) return cur;
        cur = cur->objects_next;
    }
}

static object_t *get_fileid(const file_id_t *id) {
    if (!fileids_count) return NULL;

    uint64_t hash = hash_blob(id, sizeof(*id));
    object_t *cur = objects_table[hash % objects_capacity];

    for (;;) {
        if (!cur || (cur->id_hash == hash && !memcmp(&cur->id, id, sizeof(*id)))) return cur;
        cur = cur->objects_next;
    }
}

static bool maybe_expand_objects(void) {
    if (objects_count < objects_capacity - (objects_capacity / 4)) return true;

    size_t new_cap = objects_capacity ? objects_capacity * 2 : 8;
    object_t **new_table = calloc(new_cap, sizeof(*new_table));
    if (unlikely(!new_table)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return false;
    }

    for (size_t i = 0; i < objects_capacity; i++) {
        object_t *cur = objects_table[i];

        while (cur) {
            object_t *next = cur->objects_next;

            size_t bucket = cur->hash % new_cap;
            cur->objects_prev = NULL;
            cur->objects_next = new_table[bucket];
            if (cur->objects_next) cur->objects_next->objects_prev = cur;
            new_table[bucket] = cur;

            cur = next;
        }
    }

    free(objects_table);
    objects_table = new_table;
    objects_capacity = new_cap;
    return true;
}

static bool maybe_expand_fileids(void) {
    if (fileids_count < fileids_capacity - (fileids_capacity / 4)) return true;

    size_t new_cap = fileids_capacity ? fileids_capacity * 2 : 8;
    object_t **new_table = calloc(new_cap, sizeof(*new_table));
    if (unlikely(!new_table)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return false;
    }

    for (size_t i = 0; i < fileids_capacity; i++) {
        object_t *cur = fileids_table[i];

        while (cur) {
            object_t *next = cur->fileids_next;

            size_t bucket = cur->id_hash % new_cap;
            cur->fileids_prev = NULL;
            cur->fileids_next = new_table[bucket];
            if (cur->fileids_next) cur->fileids_next->fileids_prev = cur;
            new_table[bucket] = cur;

            cur = next;
        }
    }

    free(fileids_table);
    fileids_table = new_table;
    fileids_capacity = new_cap;
    return true;
}

static void remove_object(object_t *object) {
    if (object->soname) {
        if (object->objects_prev) object->objects_prev->objects_next = object->objects_next;
        else objects_table[object->hash % objects_capacity] = object->objects_next;

        if (object->objects_next) object->objects_next->objects_prev = object->objects_prev;

        objects_count -= 1;
    }

    if (object->have_file_id) {
        if (object->fileids_prev) object->fileids_prev->fileids_next = object->fileids_next;
        else fileids_table[object->id_hash % fileids_capacity] = object->fileids_next;

        if (object->fileids_next) object->fileids_next->fileids_prev = object->fileids_prev;

        fileids_count -= 1;
    }

    if (object->prev) object->prev->next = object->next;
    else object_list = object->next;

    if (object->next) object->next->prev = object->prev;

    if (object->global) {
        if (object->global_prev) object->global_prev->global_prev = object->global_next;
        else global_head = object->global_next;

        if (object->global_next) object->global_next->global_prev = object->global_prev;
        else global_tail = object->global_prev;
    }
}

static char *do_substitutions(const char *str, size_t len, object_t *source) {
    char *buf = malloc(len + 1);
    if (unlikely(!buf)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return NULL;
    }
    memcpy(buf, str, len);
    buf[len] = 0;
    size_t buf_len = len;

    char *cur = buf;

    while (*cur) {
        char *start = strchr(cur, '$');
        if (!start) break;
        char *name = &start[1];
        char *end;
        char *next;

        if (*name == '{') {
            end = strchr(name, '}');

            if (!end) {
                cur++;
                continue;
            }

            next = end + 1;
        } else {
            end = name;
            while (isalnum(*end) || *end == '_') end++;
            next = end;
        }

        const char *replacement = NULL;
        size_t replacement_len = 0;
        size_t len = end - name;

        if (len == 6 && !memcmp(name, "ORIGIN", 6)) {
            if (secure) {
                free(buf);
                rtld_last_error = RTLD_NO_ORIGIN_SECURE;
                return NULL;
            }

            if (!source->path) {
                free(buf);
                rtld_last_error = RTLD_NO_ORIGIN;
                return NULL;
            }

            replacement = source->path;
            replacement_len = source->origin_len;
        } else {
            free(buf);
            rtld_last_error = RTLD_OS(EINVAL);
            return NULL;
        }

        size_t offset = cur - buf;
        char *new_buf = realloc(buf, buf_len + (replacement_len - len) + 1);

        if (unlikely(!new_buf)) {
            free(buf);
            rtld_last_error = RTLD_OS(ENOMEM);
            return NULL;
        }

        buf = new_buf;
        cur = new_buf + offset;

        memmove(cur + replacement_len, next, buf_len - (next - buf) + 1);
        memcpy(cur, replacement, replacement_len);

        cur = cur + replacement_len;
        buf_len += replacement_len - len;
    }

    return buf;
}

static void cleanup_part_resolve(obj_init_ctx_t *ctx, char **runpath, size_t nrunpath, size_t rpath_start) {
    for (size_t i = 0; i < nrunpath; i++) {
        free(runpath[i]);
    }

    free(runpath);

    while (rpath_start < ctx->nrpath) {
        free(ctx->rpath[--ctx->nrpath]);
    }
}

static bool add_to_path_vector(
        char ***vector,
        size_t *count,
        size_t *capacity,
        const char *src,
        const char *send,
        object_t *obj
) {
    if (!send) send = src + strlen(src);

    while (src < send) {
        const char *end = strchr(src, ':');
        const char *next;

        if (end && end < send) {
            next = end + 1;
        } else {
            end = send;
            next = end;
        }

        char *path = do_substitutions(src, end - src, obj);
        if (unlikely(!path)) return false;

        if (*count == *capacity) {
            size_t new_cap = *capacity ? *capacity * 2 : 8;
            void *new_buf = realloc(*vector, sizeof(**vector) * new_cap);

            if (unlikely(!new_buf)) {
                free(path);
                rtld_last_error = RTLD_OS(ENOMEM);
                return false;
            }

            *vector = new_buf;
            *capacity = new_cap;
        }

        (*vector)[(*count)++] = path;

        src = next;
    }

    return true;
}

void rtld_init_paths(void) {
    if (secure) return;

    while (ld_library_path) {
        const char *end = strchr(ld_library_path, ';');
        const char *next;

        if (end) {
            next = end + 1;
        } else {
            end = ld_library_path + strlen(ld_library_path);
            next = NULL;
        }

        if (unlikely(!add_to_path_vector(&libpath, &nlibpath, &clibpath, ld_library_path, end, NULL))) {
            panic("rtld: failed to build LD_LIBRARY_PATH vector: %s\n", dlerror());
        }

        ld_library_path = next;
    }
}

static char *fd_to_path(int fd, size_t *origin_len) {
    size_t buf_siz = 128;
    char *path = malloc(buf_siz);

    if (unlikely(!path)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return NULL;
    }

    for (;;) {
        hydrogen_ret_t ret = hydrogen_fs_fpath(fd, path, buf_siz - 1);

        if (unlikely(ret.error)) {
            free(path);
            rtld_last_error = RTLD_OS(ret.error);
            return NULL;
        }

        if (ret.integer < buf_siz) {
            path[ret.integer] = 0;
            *origin_len = ret.integer;

            for (;;) {
                if (path[--*origin_len] == '/') break;
            }

            if (*origin_len == 0) *origin_len = 1;
            break;
        }

        buf_siz *= 2;
        void *new_buf = malloc(buf_siz);
        free(path);
        path = new_buf;

        if (unlikely(!new_buf)) {
            rtld_last_error = RTLD_OS(ENOMEM);
            return NULL;
        }
    }

    return path;
}

extern const void __plibc_rtld_lazy_thunk;

object_t *create_object(
        int fd,
        file_id_t *id,
        uintptr_t base,
        const void *phdrs,
        size_t phent,
        size_t phnum,
        int flags,
        obj_init_ctx_t *ctx
) {
    object_t *obj = calloc(1, sizeof(*obj));
    if (unlikely(!obj)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return NULL;
    }

    obj->references = 1;
    obj->base = base;
    obj->phdrs = phdrs;
    obj->phdr_entry_size = phent;
    obj->phdr_count = phnum;
    obj->phdrs_heap = flags & RTLD_PHDRS_HEAP;
    obj->relocated = flags & RTLD_INITIALIZED;
    obj->plt_reloc = flags & RTLD_INITIALIZED;
    obj->bound = flags & RTLD_INITIALIZED;
    obj->initialized = flags & RTLD_INITIALIZED;

    if (fd != HYDROGEN_INVALID_HANDLE) {
        obj->have_file_id = true;

        if (id) {
            obj->id = *id;
        } else {
            hydrogen_file_information_t info;
            int error = hydrogen_fs_fstat(fd, &info);

            if (unlikely(error)) {
                free(obj);
                rtld_last_error = RTLD_OS(error);
                return NULL;
            }

            obj->id.fs = info.filesystem_id;
            obj->id.id = info.id;
        }

        obj->id_hash = hash_blob(&obj->id, sizeof(obj->id));
        obj->path = fd_to_path(fd, &obj->origin_len);

        if (unlikely(!obj->path)) {
            free(obj);
            return NULL;
        }
    }

    uintptr_t dynamic_virt = 0;
    uintptr_t virt_head = UINTPTR_MAX;
    uintptr_t virt_tail = 0;
    uintptr_t tls_virt = 0;

    for (size_t i = 0; i < phnum; i++) {
        const elf_phdr_t *phdr = phdrs + i * phent;

        switch (phdr->p_type) {
        case PT_LOAD:
            if (phdr->p_memsz == 0) continue;

            uintptr_t head = phdr->p_vaddr;
            uintptr_t tail = phdr->p_vaddr + (phdr->p_memsz - 1);

            if (head > tail) {
                free(obj->path);
                free(obj);
                rtld_last_error = RTLD_INVALID_IMAGE;
                return NULL;
            }

            if (head < virt_head) virt_head = head;
            if (tail > virt_tail) virt_tail = tail;
            break;
        case PT_DYNAMIC: dynamic_virt = phdr->p_vaddr; break;
        case PT_PHDR: obj->base = base = (uintptr_t)phdrs - phdr->p_vaddr; break;
        case PT_TLS:
            tls_virt = phdr->p_vaddr;
            obj->tls_init_image_size = phdr->p_filesz;
            obj->tls_size = phdr->p_memsz;
            obj->tls_align = phdr->p_align;
            break;
        }
    }

    if (virt_head <= virt_tail) {
        virt_head &= ~(hydrogen_page_size - 1);
        virt_tail |= hydrogen_page_size - 1;

        obj->memory = virt_head + obj->base;
        obj->mem_size = virt_tail - virt_head + 1;
    }

    obj->dynamic = (const void *)(dynamic_virt + obj->base);
    obj->tls_init_image = (const void *)(tls_virt + obj->base);

    size_t rpath_offset = 0, soname_offset = 0, runpath_offset = 0;

    for (const elf_dyn_t *cur = obj->dynamic; cur->d_tag != DT_NULL; cur++) {
        switch (cur->d_tag) {
        case DT_NEEDED: obj->num_deps += 1; break;
        case DT_PLTRELSZ: obj->pltrel.size = cur->d_un.d_val; break;
        case DT_PLTGOT: obj->pltgot = (void *)(cur->d_un.d_ptr + base); break;
        case DT_HASH: {
            const uint32_t *data = (void *)(cur->d_un.d_ptr + base);
            obj->hash_table.num_buckets = data[0];
            obj->hash_table.buckets = &data[2];
            obj->hash_table.chain = &obj->hash_table.buckets[obj->hash_table.num_buckets];
            break;
        }
        case DT_STRTAB: obj->strtab = (void *)(cur->d_un.d_ptr + base); break;
        case DT_SYMTAB: obj->symtab.data = (void *)(cur->d_un.d_ptr + base); break;
        case DT_RELA: obj->rela.data = (void *)(cur->d_un.d_ptr + base); break;
        case DT_RELASZ: obj->rela.size = cur->d_un.d_val; break;
        case DT_RELAENT: obj->rela.entry_size = cur->d_un.d_val; break;
        case DT_SYMENT: obj->symtab.entry_size = cur->d_un.d_val; break;
        case DT_INIT: obj->init = (void *)(cur->d_un.d_ptr + base); break;
        case DT_FINI: obj->fini = (void *)(cur->d_un.d_ptr + base); break;
        case DT_SONAME: soname_offset = cur->d_un.d_val; break;
        case DT_RPATH: rpath_offset = cur->d_un.d_val; break;
        case DT_SYMBOLIC: obj->symbolic = true; break;
        case DT_REL: obj->rel.data = (void *)(cur->d_un.d_ptr + base); break;
        case DT_RELSZ: obj->rel.size = cur->d_un.d_val; break;
        case DT_RELENT: obj->rel.entry_size = cur->d_un.d_val; break;
        case DT_PLTREL:
            switch (cur->d_un.d_val) {
            case DT_RELA: obj->pltrel.entry_size = sizeof(elf_rela_t); break;
            case DT_REL: obj->pltrel.entry_size = sizeof(elf_rel_t); break;
            default:
                free(obj->path);
                free(obj);
                rtld_last_error = RTLD_INVALID_IMAGE;
                return NULL;
            }
            break;
        case DT_TEXTREL: obj->textrel = true; break;
        case DT_JMPREL: obj->pltrel.data = (void *)(cur->d_un.d_ptr + base); break;
        case DT_BIND_NOW: obj->bind_now = true; break;
        case DT_INIT_ARRAY: obj->init_array.functions = (void *)(cur->d_un.d_ptr + base); break;
        case DT_FINI_ARRAY: obj->fini_array.functions = (void *)(cur->d_un.d_ptr + base); break;
        case DT_INIT_ARRAYSZ: obj->init_array.size = cur->d_un.d_val; break;
        case DT_FINI_ARRAYSZ: obj->fini_array.size = cur->d_un.d_val; break;
        case DT_RUNPATH: runpath_offset = cur->d_un.d_val; break;
        case DT_FLAGS:
            if (cur->d_un.d_val & DF_SYMBOLIC) obj->symbolic = true;
            if (cur->d_un.d_val & DF_TEXTREL) obj->textrel = true;
            if (cur->d_un.d_val & DF_BIND_NOW) obj->bind_now = true;
            break;
        case DT_PREINIT_ARRAY: obj->preinit_array.functions = (void *)(cur->d_un.d_ptr + base); break;
        case DT_PREINIT_ARRAYSZ: obj->preinit_array.size = cur->d_un.d_val; break;
        }
    }

    if (soname_offset) {
        obj->soname = obj->strtab + soname_offset;
        obj->hash = hash_blob(obj->soname, strlen(obj->soname));
    }

    if (rpath_offset) obj->rpath = obj->strtab + rpath_offset;
    if (runpath_offset) obj->runpath = obj->strtab + runpath_offset;

    obj->dependencies = calloc(obj->num_deps, sizeof(*obj->dependencies));

    obj->prev = NULL;
    obj->next = object_list;
    if (obj->next) obj->next->prev = object_list;
    object_list = obj;

    if (obj->soname) {
        if (!maybe_expand_objects()) {
            obj->phdrs = NULL;
            obj->soname = NULL;
            obj->have_file_id = false;
            obj->num_deps = 0;
            obj_deref(obj);
            return NULL;
        }

        size_t bucket = obj->hash % objects_capacity;
        obj->objects_prev = NULL;
        obj->objects_next = objects_table[bucket];
        if (obj->objects_next) obj->objects_next->objects_prev = obj;
        objects_table[bucket] = obj;

        objects_count += 1;
    }

    if (obj->have_file_id) {
        if (!maybe_expand_fileids()) {
            obj->phdrs = NULL;
            obj->have_file_id = false;
            obj->num_deps = 0;
            obj_deref(obj);
            return NULL;
        }

        size_t bucket = obj->id_hash % fileids_capacity;
        obj->fileids_prev = NULL;
        obj->fileids_next = fileids_table[bucket];
        if (obj->fileids_next) obj->fileids_next->fileids_prev = obj;
        fileids_table[bucket] = obj;

        fileids_count += 1;
    }

    if (!obj->relocated && obj->pltgot) {
        const void **got = obj->pltgot;
        got[1] = obj;
        got[2] = &__plibc_rtld_lazy_thunk;
    }

    if (obj->num_deps != 0) {
        char **runpath = NULL;
        size_t nrunpath = 0;
        size_t crunpath = 0;
        size_t rpath_start = ctx->nrpath;

        if (obj->runpath && unlikely(!add_to_path_vector(&runpath, &nrunpath, &crunpath, obj->runpath, NULL, obj))) {
            cleanup_part_resolve(ctx, runpath, nrunpath, rpath_start);
            obj->phdrs = NULL;
            obj->num_deps = 0;
            obj_deref(obj);
            return NULL;
        }

        if (obj->rpath &&
            unlikely(!add_to_path_vector(&ctx->rpath, &ctx->nrpath, &ctx->crpath, obj->rpath, NULL, obj))) {
            cleanup_part_resolve(ctx, runpath, nrunpath, rpath_start);
            obj->phdrs = NULL;
            obj->num_deps = 0;
            obj_deref(obj);
            return NULL;
        }

        size_t index = 0;

        for (const elf_dyn_t *dyn = obj->dynamic; index < obj->num_deps && dyn->d_tag != DT_NULL; dyn++) {
            if (dyn->d_tag != DT_NEEDED) continue;

            const char *name = obj->strtab + dyn->d_un.d_val;
            object_t *dep = resolve_object(name, flags & ~RTLD_PHDRS_HEAP, obj, ctx, runpath, nrunpath);

            if (unlikely(!dep)) {
                if (flags & RTLD_DIAGNOSTICS) {
                    fprintf(stderr, "rtld: failed to resolve dependency %s: %s\n", name, dlerror());
                }

                cleanup_part_resolve(ctx, runpath, nrunpath, rpath_start);
                obj->phdrs = NULL;
                obj->num_deps = index;
                obj_deref(obj);
                return NULL;
            }

            obj->dependencies[index++] = dep;
            dep->unterminated_dependents += 1;
        }

        cleanup_part_resolve(ctx, runpath, nrunpath, rpath_start);
    }

    if (obj->tls_size) {
        if (flags & RTLD_INITIALIZED) {
            obj->tls_module = subject_phdrs ? 0 : 1;
        } else if (flags & RTLD_ROOT_OBJECT) {
            obj->tls_module = 1;

            size_t align = obj->tls_align;
            if (align == 0) align = 1;

            if (align & (align - 1)) {
                rtld_last_error = RTLD_INVALID_IMAGE;
                obj->phdrs = NULL;
                obj_deref(obj);
                return NULL;
            }

            if (obj->tls_align > root_tls_align) root_tls_align = obj->tls_align;
            root_tls_size = (root_tls_size + obj->tls_size + (obj->tls_align - 1)) & ~(obj->tls_align - 1);

            obj->tls_offset = -root_tls_size;
        } else {
            obj->tls_module = next_tls_module++;
        }
    }

    obj->resolved = true;
    if (fd != HYDROGEN_INVALID_HANDLE) hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, fd);
    return obj;
}

bool obj_init_for_search(obj_init_ctx_t *ctx, object_t *src, char ***runpath, size_t *nrunpath) {
    size_t crunpath = 0;

    if (src->runpath && unlikely(!add_to_path_vector(runpath, nrunpath, &crunpath, src->runpath, NULL, src))) {
        return false;
    }

    if (src->rpath && unlikely(!add_to_path_vector(&ctx->rpath, &ctx->nrpath, &ctx->crpath, src->rpath, NULL, src))) {
        return false;
    }

    return true;
}

typedef struct {
    object_t *object;
    const elf_sym_t *symbol;
} resolved_symbol_t;

static uint32_t elf_hash(const char *name) {
    uint32_t hash = 0;

    for (;;) {
        unsigned char c = *name++;
        if (!c) break;
        hash = (hash << 4) + c;

        uint32_t top = hash & 0xf0000000;
        if (top) hash ^= top >> 24;
        hash &= ~top;
    }

    return hash;
}

static const elf_sym_t *get_symbol_in_object(object_t *obj, const char *name) {
    if (!obj->hash_table.num_buckets) return NULL;

    uint32_t hash = elf_hash(name);
    uint32_t index = obj->hash_table.buckets[hash % obj->hash_table.num_buckets];

    while (index != STN_UNDEF) {
        const elf_sym_t *sym = obj->symtab.data + index * obj->symtab.entry_size;

        if (!strcmp(obj->strtab + sym->st_name, name)) {
            return sym;
        }

        index = obj->hash_table.chain[index];
    }

    return NULL;
}

static bool is_real_symbol(const elf_sym_t *sym) {
    return sym->st_shndx != SHN_UNDEF || sym->st_value != 0;
}

static bool resolve_symbol(
        resolved_symbol_t *out,
        search_list_t *search_list,
        const char *name,
        object_t *src_obj,
        const elf_sym_t *src_sym,
        bool include_global
) {
    object_t *cur_obj = NULL;
    const elf_sym_t *cur_sym = NULL;
    bool have_weak = false;

    if (src_sym != NULL && is_real_symbol(src_sym)) {
        cur_obj = src_obj;
        cur_sym = src_sym;

        if (src_obj->symbolic) {
            if (ELF_ST_BIND(src_sym->st_info) != STB_WEAK) goto done;
            have_weak = true;
        }
    }

    if (include_global) {
        for (object_t *obj = global_head; obj != NULL; obj = obj->global_next) {
            const elf_sym_t *sym = get_symbol_in_object(obj, name);
            if (!sym || !is_real_symbol(sym)) continue;
            if (ELF64_ST_BIND(sym->st_info) == STB_WEAK && have_weak) continue;

            cur_obj = obj;
            cur_sym = sym;
            if (ELF64_ST_BIND(sym->st_info) != STB_WEAK) goto done;
            have_weak = true;
        }
    }

    for (object_t *obj = search_list->head; obj != NULL; obj = obj->search_next) {
        const elf_sym_t *sym = get_symbol_in_object(obj, name);
        if (!sym || !is_real_symbol(sym)) continue;
        if (ELF64_ST_BIND(sym->st_info) == STB_WEAK && have_weak) continue;

        cur_obj = obj;
        cur_sym = sym;
        if (ELF64_ST_BIND(sym->st_info) != STB_WEAK) goto done;
        have_weak = true;
    }

done:
    if (!cur_sym) return false;

    out->object = cur_obj;
    out->symbol = cur_sym;
    return true;
}

static uintptr_t get_symbol_address(resolved_symbol_t *symbol) {
    if (!symbol->symbol) return 0;

    uintptr_t addr = symbol->symbol->st_value + symbol->object->base;

    if (ELF_ST_TYPE(symbol->symbol->st_info) == STT_GNU_IFUNC) {
        addr = (uintptr_t)((void *(*)(void))addr)();
    }

    return addr;
}

static size_t next_search_id = 1;

static void build_search_list(search_list_t *out, object_t *root) {
    size_t id = next_search_id++;

    out->head = out->tail = root;
    root->search_next = NULL;

    for (object_t *cur = out->head; cur != NULL; cur = cur->search_next) {
        for (size_t i = 0; i < cur->num_deps; i++) {
            object_t *dep = cur->dependencies[i];
            if (dep->search_id == id) continue;

            dep->search_id = id;
            dep->search_next = NULL;
            out->tail->search_next = dep;
            out->tail = dep;
        }
    }
}

USED uintptr_t __plibc_rtld_lazy(object_t *obj, size_t idx) {
    const elf_rela_t *rel = obj->pltrel.data + idx * obj->pltrel.entry_size;
    uintptr_t *ptr = (uintptr_t *)(rel->r_offset + obj->base);

    const elf_sym_t *src_sym = obj->symtab.data + ELF_R_SYM(rel->r_info) * obj->symtab.entry_size;
    resolved_symbol_t symbol = {};
    search_list_t list = {};
    build_search_list(&list, obj);

    if (!resolve_symbol(&symbol, &list, obj->strtab + src_sym->st_name, obj, src_sym, true) &&
        ELF_ST_BIND(src_sym->st_info) != STB_WEAK) {
        panic("rtld: unresolved lazy reference to symbol '%s'\n", obj->strtab + src_sym->st_name);
    }

    uintptr_t addr = get_symbol_address(&symbol);
    *ptr = addr;
    return addr;
}

#define ADDEND(type, ptr, rel, rela) ((rela) ? (type)((const elf_rela_t *)(rel))->r_addend : *(type *)ptr)

static bool do_reloc(object_t *obj, search_list_t *list, const elf_rel_t *rel, bool rela, bool bind) {
    void *ptr = (void *)(rel->r_offset + obj->base);
    uint32_t type = ELF_R_TYPE(rel->r_info);
    uint32_t index = ELF_R_SYM(rel->r_info);
    resolved_symbol_t sym = {};

    if (index != STN_UNDEF) {
        const elf_sym_t *src_sym = obj->symtab.data + index * obj->symtab.entry_size;

        if (!resolve_symbol(&sym, list, obj->strtab + src_sym->st_name, obj, src_sym, true) &&
            ELF_ST_BIND(src_sym->st_info) != STB_WEAK) {
            rtld_last_error = RTLD_UNRESOLVED_SYMBOL;
            return false;
        }
    }

    switch (type) {
    case R_NONE: break;
    case R_POINTER: *(uintptr_t *)ptr = get_symbol_address(&sym) + ADDEND(uintptr_t, ptr, rel, rela); break;
    case R_COPY: memcpy(ptr, (void *)get_symbol_address(&sym), sym.symbol->st_size); break;
    case R_GLOB_DAT: *(uintptr_t *)ptr = get_symbol_address(&sym); break;
    case R_JUMP_SLOT:
        if (bind) *(uintptr_t *)ptr = get_symbol_address(&sym);
        else *(uintptr_t *)ptr += obj->base;
        break;
    case R_RELATIVE: *(uintptr_t *)ptr = obj->base + ADDEND(uintptr_t, ptr, rel, rela); break;
    case R_IRELATIVE:
        *(uintptr_t *)ptr = (uintptr_t)((void *(*)(void))(obj->base + ADDEND(uintptr_t, ptr, rel, rela)))();
        break;
    case R_DTPMOD: *(uintptr_t *)ptr = sym.symbol ? sym.object->tls_module : SIZE_MAX; break;
    case R_TPOFF:
        if (sym.symbol != NULL && sym.object->tls_module >= 2) {
            // Can't use static TLS for this object
            rtld_last_error = RTLD_INVALID_IMAGE;
            return false;
        }
        // fall through
    case R_DTPOFF: *(uintptr_t *)ptr = sym.symbol ? sym.object->tls_offset + sym.symbol->st_value : 0; break;
    default: rtld_last_error = RTLD_INVALID_IMAGE; return false;
    }

    return true;
}

static bool relocate_object_table(
        object_t *obj,
        search_list_t *list,
        const relocation_table_t *table,
        bool rela,
        bool bind
) {
    bool have_late_relocations = false;

    for (size_t i = 0; i < table->size; i += table->entry_size) {
        const elf_rel_t *cur = table->data + i;
        uint32_t type = ELF_R_TYPE(cur->r_info);

        if (type == R_COPY || type == R_IRELATIVE) {
            have_late_relocations = true;
            continue;
        }

        if (unlikely(!do_reloc(obj, list, cur, rela, bind))) return false;
    }

    if (!have_late_relocations) return true;

    for (size_t i = 0; i < table->size; i += table->entry_size) {
        const elf_rel_t *cur = table->data + i;
        uint32_t type = ELF_R_TYPE(cur->r_info);

        if (type != R_COPY && type != R_IRELATIVE) continue;

        if (unlikely(!do_reloc(obj, list, cur, rela, bind))) return false;
    }

    return true;
}

static bool relocate_object_data(object_t *obj, search_list_t *list) {
    if (obj->relocated) return true;
    if (unlikely(!relocate_object_table(obj, list, &obj->rel, false, false))) return false;
    if (unlikely(!relocate_object_table(obj, list, &obj->rela, true, false))) return false;
    obj->relocated = true;
    return true;
}

static bool relocate_object_code(object_t *obj, search_list_t *list, bool bind) {
    bool rela = obj->pltrel.entry_size >= sizeof(elf_rela_t);

    if (!obj->plt_reloc) {
        if (unlikely(!relocate_object_table(obj, list, &obj->pltrel, rela, false))) {
            return false;
        }

        obj->plt_reloc = true;
    }

    if (obj->bound || !bind) return true;

    if (unlikely(!relocate_object_table(obj, list, &obj->pltrel, rela, true))) {
        return false;
    }

    obj->bound = true;
    return true;
}

static void call_init_function(initfn_t fn) {
    fn(rtld_argc, rtld_argv, rtld_envp);
}

static void call_init_array(const init_array_t *array) {
    size_t count = array->size / sizeof(*array->functions);

    for (size_t i = 0; i < count; i++) {
        call_init_function(array->functions[i]);
    }
}

static void call_fini_array(const init_array_t *array) {
    size_t count = array->size / sizeof(*array->functions);

    for (size_t i = count; i > 0; i--) {
        call_init_function(array->functions[i - 1]);
    }
}

static void call_init_functions(object_t *obj) {
    if (obj->initialized) return;

    if (obj == root_object) call_init_array(&obj->preinit_array);

    for (size_t i = 0; i < obj->num_deps; i++) {
        call_init_functions(obj->dependencies[i]);
    }

    if (obj->init) call_init_function(obj->init);
    call_init_array(&obj->init_array);

    obj->initialized = true;
}

static tcb_t *allocate_tcb(void) {
    if (root_tls_align < _Alignof(tcb_t)) root_tls_align = _Alignof(tcb_t);

    size_t tcb_offset = (root_tls_size + (root_tls_align - 1)) & ~(root_tls_align - 1);
    size_t area_size = (tcb_offset + sizeof(tcb_t) + (root_tls_align - 1)) & ~(root_tls_align - 1);
    void *area = aligned_alloc(root_tls_align, area_size);
    if (unlikely(!area)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return NULL;
    }

    tcb_t *tcb = area + tcb_offset;
    memset(tcb, 0, sizeof(*tcb));
    tcb->self = tcb;
    tcb->dtv_size = 2;
    tcb->dtv = malloc(sizeof(*tcb->dtv) * tcb->dtv_size);
    if (unlikely(!tcb->dtv)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        return NULL;
    }

    tcb->dtv[0] = rtld_tls_area;
    tcb->dtv[1] = tcb;

    for (object_t *obj = object_list; obj != NULL; obj = obj->next) {
        if (!obj->tls_size) continue;
        if (obj->tls_module != 1) continue;

        memcpy((void *)tcb + obj->tls_offset, obj->tls_init_image, obj->tls_init_image_size);
        memset((void *)tcb + obj->tls_offset + obj->tls_init_image_size, 0, obj->tls_size - obj->tls_init_image_size);
    }

    return tcb;
}

static void transition_tls(void) {
    tcb_t *tcb = allocate_tcb();
    if (unlikely(!tcb)) panic("rtld: failed to allocate initial tcb: %s\n", dlerror());
    arch_rtld_set_tcb(tcb);
}

bool obj_init_finalize(object_t *root, int flags) {
    size_t id = next_search_id++;

    search_list_t list = {root, root};
    root->search_next = NULL;

    if (flags & RTLD_ROOT_OBJECT) {
        root_object = root;
        obj_ref(root);
    }

    // build search list
    for (object_t *cur = list.head; cur != NULL; cur = cur->search_next) {
        if ((flags & RTLD_GLOBAL) != 0 && root_object != NULL && !cur->global) {
            cur->global = true;

            cur->global_prev = global_tail;
            cur->global_next = NULL;
            if (cur->global_prev) cur->global_prev->global_next = cur;
            else global_head = cur;
            global_tail = cur;
        }

        for (size_t i = 0; i < cur->num_deps; i++) {
            object_t *dep = cur->dependencies[i];
            if (dep->search_id == id) continue;

            dep->search_id = id;
            dep->search_next = NULL;
            list.tail->search_next = dep;
            list.tail = dep;
        }
    }

    // relocate objects
    for (object_t *cur = list.head; cur != NULL; cur = cur->search_next) {
        if (!relocate_object_data(cur, &list)) {
            return false;
        }

        if (!relocate_object_code(
                    cur,
                    &list,
                    force_bind_now || (flags & RTLD_NOW) != 0 || cur->bind_now || cur->textrel
            )) {
            return false;
        }

        if (cur->textrel) {
            // Fix permissions of textrel object

            for (size_t i = 0; i < cur->phdr_count; i++) {
                const elf_phdr_t *phdr = cur->phdrs + i * cur->phdr_entry_size;
                if (phdr->p_type != PT_LOAD) continue;
                if (phdr->p_memsz == 0) continue;
                if (phdr->p_flags & PF_W) continue;

                uintptr_t map_head = phdr->p_vaddr + cur->base;
                uintptr_t map_tail = (map_head + (phdr->p_memsz - 1)) | (hydrogen_page_size - 1);
                map_head &= ~(hydrogen_page_size - 1);

                uint32_t new_flags = 0;
                if (phdr->p_flags & PF_R) new_flags |= HYDROGEN_MEM_READ;
                if (phdr->p_flags & PF_X) new_flags |= HYDROGEN_MEM_EXEC;

                int error = hydrogen_vmm_remap(HYDROGEN_THIS_VMM, map_head, map_tail - map_head + 1, new_flags);

                if (unlikely(error)) {
                    rtld_last_error = RTLD_OS(error);
                    return false;
                }
            }

            cur->textrel = false;
        }
    }

    if ((flags & RTLD_ROOT_OBJECT) != 0 && subject_phdrs != NULL) transition_tls();

    call_init_functions(root);
    return true;
}

object_t *address_to_object(uintptr_t address) {
    for (object_t *cur = object_list; cur != NULL; cur = cur->next) {
        if (cur->memory <= address && address < cur->memory + cur->mem_size) {
            return cur;
        }
    }

    rtld_last_error = RTLD_NO_IMAGE_FOR_ADDRESS;
    return NULL;
}

static bool read_fully(int fd, void *buffer, size_t size, uint64_t position) {
    while (size) {
        hydrogen_ret_t ret = hydrogen_fs_pread(fd, buffer, size, position);

        if (unlikely(ret.error)) {
            rtld_last_error = RTLD_OS(ret.error);
            return false;
        }

        if (unlikely(ret.integer == 0)) {
            rtld_last_error = RTLD_INVALID_IMAGE;
            return false;
        }

        buffer += ret.integer;
        position += ret.integer;
        size -= ret.integer;
    }

    return true;
}

static int load_object_from_path(object_t **out, const char *path, obj_init_ctx_t *ctx, int flags) {
    hydrogen_ret_t ret = hydrogen_fs_open(HYDROGEN_INVALID_HANDLE, path, strlen(path), O_RDONLY, 0);

    if (ret.error) {
        rtld_last_error = RTLD_OS(ret.error);
        return likely(ret.error == ENOENT) ? 0 : -1;
    }

    int fd = ret.integer;

    hydrogen_file_information_t info;
    int error = hydrogen_fs_fstat(fd, &info);
    if (unlikely(error)) {
        rtld_last_error = RTLD_OS(error);
        goto err;
    }

    file_id_t id = {info.filesystem_id, info.id};
    object_t *obj = get_fileid(&id);

    if (obj) {
        obj_ref(obj);
        *out = obj;
        return 1;
    }

    elf_ehdr_t header;

    if (!read_fully(fd, &header, sizeof(header), 0)) {
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, fd);
        return likely(rtld_last_error == RTLD_INVALID_IMAGE) ? 0 : -1;
    }

    if (memcmp(header.e_ident, elf_native_ident, sizeof(elf_native_ident))) {
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, fd);
        rtld_last_error = RTLD_INVALID_IMAGE;
        return 0;
    }

    if (header.e_machine != EM_NATIVE || header.e_type != ET_DYN || header.e_version != EV_CURRENT) {
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, fd);
        rtld_last_error = RTLD_INVALID_IMAGE;
        return 0;
    }

    // Valid image, all errors from now on are fatal

    size_t phdr_size = (size_t)header.e_phnum * header.e_phentsize;
    void *phdrs = malloc(phdr_size);

    if (unlikely(!phdrs)) {
        rtld_last_error = RTLD_OS(ENOMEM);
        goto err;
    }

    if (unlikely(!read_fully(fd, phdrs, phdr_size, header.e_phoff))) {
        goto phdr_err;
    }

    uintptr_t virt_head = UINTPTR_MAX;
    uintptr_t virt_tail = 0;
    bool textrel = false;

    for (size_t i = 0; i < header.e_phnum; i++) {
        elf_phdr_t *phdr = phdrs + i * header.e_phentsize;

        switch (phdr->p_type) {
        case PT_LOAD:
            if (phdr->p_memsz == 0) continue;

            uintptr_t head = phdr->p_vaddr;
            uintptr_t tail = head + (phdr->p_memsz - 1);

            if (head < virt_head) virt_head = head;
            if (tail > virt_tail) virt_tail = tail;

            break;
        case PT_DYNAMIC:
            if (phdr->p_memsz == 0) continue;
            elf_dyn_t *buffer = malloc(phdr->p_memsz);

            if (unlikely(!buffer)) {
                rtld_last_error = RTLD_OS(ENOMEM);
                goto phdr_err;
            }

            if (unlikely(!read_fully(fd, buffer, phdr->p_filesz, phdr->p_offset))) {
                free(buffer);
                goto phdr_err;
            }

            memset(buffer + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);

            for (elf_dyn_t *cur = buffer; cur->d_tag != DT_NULL; cur++) {
                if (cur->d_tag == DT_TEXTREL) {
                    textrel = true;
                } else if (cur->d_tag == DT_FLAGS) {
                    if ((cur->d_un.d_val & DF_TEXTREL) != 0) textrel = true;
                }
            }

            free(buffer);
            break;
        default: break;
        }
    }

    uintptr_t base = 0;

    if (virt_head <= virt_tail) {
        virt_head &= ~(hydrogen_page_size - 1);
        virt_tail |= hydrogen_page_size - 1;
        size_t image_size = virt_tail - virt_head + 1;

        ret = hydrogen_vmm_map(HYDROGEN_THIS_VMM, 0, image_size, HYDROGEN_MEM_LAZY_RESERVE, HYDROGEN_INVALID_HANDLE, 0);

        if (unlikely(ret.error)) {
            rtld_last_error = RTLD_OS(ret.error);
            goto phdr_err;
        }

        base = ret.integer - virt_head;

        for (size_t i = 0; i < header.e_phnum; i++) {
            elf_phdr_t *phdr = phdrs + i * header.e_phentsize;
            if (phdr->p_type != PT_LOAD || phdr->p_memsz == 0) continue;

            size_t offset = phdr->p_vaddr & (hydrogen_page_size - 1);
            uintptr_t map_head = phdr->p_vaddr - offset + base;
            uintptr_t zero_tail = map_head + offset + phdr->p_memsz - 1;
            uintptr_t map_tail = zero_tail | (hydrogen_page_size - 1);

            uint32_t map_flags = HYDROGEN_MEM_EXACT | HYDROGEN_MEM_OVERWRITE;

            if ((phdr->p_flags & PF_R) != 0) map_flags |= HYDROGEN_MEM_READ;
            if ((phdr->p_flags & PF_X) != 0) map_flags |= HYDROGEN_MEM_EXEC;

            if (textrel || (phdr->p_flags & PF_W) != 0) map_flags |= HYDROGEN_MEM_WRITE;
            else map_flags |= HYDROGEN_MEM_SHARED;

            if (phdr->p_filesz != 0) {
                uint64_t map_offset = phdr->p_offset - offset;
                uintptr_t file_tail = map_head + offset + phdr->p_filesz - 1;
                uintptr_t file_map_tail = file_tail | (hydrogen_page_size - 1);
                size_t map_size = file_map_tail - map_head + 1;
                size_t extra_size = 0;

                if (file_tail < zero_tail && file_tail < file_map_tail && (map_flags & HYDROGEN_MEM_WRITE) == 0) {
                    extra_size = hydrogen_page_size;
                }

                if (map_size > extra_size) {
                    ret = hydrogen_fs_mmap(
                            fd,
                            HYDROGEN_THIS_VMM,
                            map_head,
                            map_size - extra_size,
                            map_flags,
                            map_offset
                    );

                    if (unlikely(ret.error)) {
                        rtld_last_error = RTLD_OS(ret.error);
                        goto load_err;
                    }

                    map_head += map_size -= extra_size;
                    map_offset += map_size - extra_size;
                    map_size = extra_size;
                }

                if (map_size) {
                    ret = hydrogen_fs_mmap(
                            fd,
                            HYDROGEN_THIS_VMM,
                            map_head,
                            map_size - extra_size,
                            (map_flags & ~HYDROGEN_MEM_SHARED) | HYDROGEN_MEM_WRITE,
                            map_offset
                    );

                    if (unlikely(ret.error)) {
                        rtld_last_error = RTLD_OS(ret.error);
                        goto load_err;
                    }
                }

                if (file_tail < zero_tail) {
                    memset((void *)file_tail, 0, (zero_tail < file_map_tail ? zero_tail : file_map_tail) - file_tail);
                }

                if (map_size) {
                    error = hydrogen_vmm_remap(
                            HYDROGEN_THIS_VMM,
                            map_head,
                            map_size,
                            map_flags & (HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE | HYDROGEN_MEM_EXEC)
                    );

                    if (unlikely(error)) {
                        rtld_last_error = RTLD_OS(error);
                        goto load_err;
                    }

                    map_head += map_size;
                }
            }

            if (map_head < map_tail) {
                hydrogen_ret_t ret = hydrogen_vmm_map(
                        HYDROGEN_THIS_VMM,
                        map_head,
                        map_tail - map_head + 1,
                        map_flags & ~HYDROGEN_MEM_SHARED,
                        HYDROGEN_INVALID_HANDLE,
                        0
                );

                if (unlikely(ret.error)) {
                    rtld_last_error = RTLD_OS(ret.error);
                    goto load_err;
                }
            }
        }
    }

    obj = create_object(fd, &id, base, phdrs, header.e_phentsize, header.e_phnum, flags | RTLD_PHDRS_HEAP, ctx);
    if (unlikely(!obj)) goto load_err;

    *out = obj;
    return 1;
load_err:
    if (virt_head <= virt_tail) {
        hydrogen_vmm_unmap(HYDROGEN_THIS_VMM, virt_tail + base, virt_tail - virt_head + 1);
    }
phdr_err:
    free(phdrs);
err:
    hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, fd);
    return -1;
}

static int load_object_from_paths(
        object_t **out,
        obj_init_ctx_t *ctx,
        char *name,
        size_t name_len,
        char **paths,
        size_t npaths,
        int flags
) {
    for (size_t i = 0; i < npaths; i++) {
        char *base = paths[i];
        size_t blen = strlen(base);
        size_t tlen = blen + name_len + 2;
        char *path = malloc(tlen);

        if (unlikely(!path)) {
            rtld_last_error = RTLD_OS(ENOMEM);
            return -1;
        }

        memcpy(path, base, blen);
        path[blen] = '/';
        memcpy(&path[blen + 1], name, name_len + 1);

        int ret = load_object_from_path(out, path, ctx, flags);
        free(path);
        if (ret != 0) return ret;
    }

    return 0;
}

object_t *resolve_object(
        const char *path,
        int flags,
        object_t *source,
        obj_init_ctx_t *ctx,
        char **runpath,
        size_t nrunpath
) {
    char *real_path = do_substitutions(path, strlen(path), source);
    if (unlikely(!real_path)) return NULL;

    if (strchr(real_path, '/')) {
        object_t *obj = NULL;
        load_object_from_path(&obj, real_path, ctx, flags);
        return obj;
    }

    object_t *obj = get_object(real_path);

    if (obj) {
        free(real_path);

        if (unlikely(!obj->resolved)) {
            rtld_last_error = RTLD_DEPENDENCY_CYCLE;
            return NULL;
        }

        obj_ref(obj);
        return obj;
    }

    static char *deflibpaths[] = {"/lib", "/usr/lib"};

    struct {
        char **paths;
        size_t npaths;
    } pathvecs[] = {
            {ctx->rpath, ctx->nrpath},
            {libpath, nlibpath},
            {runpath, nrunpath},
            {deflibpaths, sizeof(deflibpaths) / sizeof(*deflibpaths)},
    };

    size_t name_len = strlen(real_path);

    for (size_t i = 0; i < sizeof(pathvecs) / sizeof(*pathvecs); i++) {
        int ret = load_object_from_paths(&obj, ctx, real_path, name_len, pathvecs[i].paths, pathvecs[i].npaths, flags);

        if (ret != 0) {
            free(real_path);
            return obj;
        }
    }

    rtld_last_error = RTLD_OS(ENOENT);
    return NULL;
}

void *obj_resolve(object_t *src, const char *name) {
    resolved_symbol_t symbol = {};

    if (src->symbolic) {
        symbol.symbol = get_symbol_in_object(symbol.object, name);

        if (symbol.symbol && is_real_symbol(symbol.symbol)) {
            symbol.object = src;
            goto done;
        }

        symbol.symbol = NULL;
    }

    search_list_t list = {};
    build_search_list(&list, src);

    if (!resolve_symbol(&symbol, &list, name, NULL, NULL, false)) {
        return NULL;
    }

done:
    return (void *)get_symbol_address(&symbol);
}

object_t *get_tls_object(size_t module) {
    for (object_t *cur = object_list; cur != NULL; cur = cur->next) {
        if (cur->tls_module == module) {
            return cur;
        }
    }

    return NULL;
}

void obj_ref(object_t *obj) {
    obj->references += 1;
    __atomic_fetch_add(&obj->references, 1, __ATOMIC_ACQUIRE);
}

void obj_deref(object_t *obj) {
    if (--obj->references == 0) {
        bool just_terminated = !obj->terminated;

        if (!just_terminated) {
            obj->terminated = true;
            call_fini_array(&obj->fini_array);
            if (obj->fini) call_init_function(obj->fini);
        }

        for (size_t i = 0; i < obj->num_deps; i++) {
            if (just_terminated) obj->dependencies[i]->unterminated_dependents -= 1;
            obj_deref(obj->dependencies[i]);
        }

        remove_object(obj);

        if (obj->phdrs_heap) free((void *)obj->phdrs);

        // TODO: Free TLS areas

        free(obj->path);
        free(obj->dependencies);
        hydrogen_vmm_unmap(HYDROGEN_THIS_VMM, obj->memory, obj->mem_size);
        free(obj);
    }
}

EXPORT void run_fini_functions(void) {
    for (;;) {
        bool done = true;
        object_t *cur = object_list;

        while (cur) {
            if (!cur->terminated && cur->unterminated_dependents == 0) {
                cur->terminated = true;
                obj_ref(cur);

                call_fini_array(&cur->fini_array);
                if (cur->fini) call_init_function(cur->fini);

                for (size_t i = 0; i < cur->num_deps; i++) {
                    cur->dependencies[i]->unterminated_dependents -= 1;
                }

                object_t *next = cur->next;
                obj_deref(cur);
                cur = next;
                done = false;
            } else {
                cur = cur->next;
            }
        }

        if (done) break;
    }
}
