#ifndef _ERRNO_H
#define _ERRNO_H 1

#include <hydrogen/errno.h> /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

/* C90 */

#define errno errno

extern int errno;

#ifdef __cplusplus
};
#endif

#endif /* _ERRNO_H */
