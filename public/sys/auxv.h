#ifndef _SYS_AUXV_H
#define _SYS_AUXV_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* Misc */

#define AT_NULL 0
#define AT_SYSINFO_EHDR 33

unsigned long getauxval(unsigned long __type);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_AUXV_H */
