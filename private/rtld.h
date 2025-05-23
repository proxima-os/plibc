#pragma once

#include <stddef.h>

void run_fini_functions(void) asm("__plibc_rtld_run_fini_functions");

char *do_dlerror(void) asm("__plibc_rtld_do_dlerror");
void *do_dlopen(const char *path, int mode, void *caller) asm("__plibc_rtld_do_dlopen");
int do_dlclose(void *handle) asm("__plibc_rtld_do_dlclose");
void *do_dlsym(void *restrict handle, const char *restrict symbol) asm("__plibc_rtld_do_dlsym");
void *do_tls_get_addr(size_t module, size_t offset) asm("__plibc_rtld_do_tls_get_addr");
