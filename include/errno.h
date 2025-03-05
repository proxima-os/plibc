#ifndef _ERRNO_H
#define _ERRNO_H 1

#include <bits/features.h>
#include <hydrogen/errno.h> /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;
#define errno errno /* C90 says it's a macro */

#ifdef __cplusplus
};
#endif

#endif /* _ERRNO_H */
