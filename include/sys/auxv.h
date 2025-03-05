#ifndef _SYS_AUXV_H
#define _SYS_AUXV_H 1

#include <bits/features.h>
#include <elf.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned long getauxval(unsigned long __type);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_AUXV_H */
