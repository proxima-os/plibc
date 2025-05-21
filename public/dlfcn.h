#ifndef _DLFCN_H
#define _DLFCN_H 1

#include <bits/features.h>

#ifndef _PROXIMA_SOURCE
#error "dlfcn.h requires _PROXIMA_SOURCE"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY 0
#define RTLD_LOCAL 0
#define RTLD_NOW (1 << 0)
#define RTLD_GLOBAL (1 << 1)

char *dlerror(void);
void *dlopen(const char *__file, int __mode);
int dlclose(void *__handle);
void *dlsym(void *__restrict __handle, const char *__restrict __name);

#ifdef __cplusplus
};
#endif

#endif /* _DLFCN_H */
