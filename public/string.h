#ifndef _STRING_H
#define _STRING_H 1

#include <bits/NULL.h>
#include <bits/features.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *__restrict __s1, const void *__restrict __s2, size_t __n);
void *memmove(void *__s1, const void *__s2, size_t __n);
char *strcpy(char *__restrict __s1, const char *__restrict __s2);
char *strncpy(char *__s1, const char *__restrict __s2, size_t __n);
char *strcat(char *__restrict __s1, const char *__restrict __s2);
char *strncat(char *__s1, const char *__restrict __s2, size_t __n);
int memcmp(const void *__s1, const void *__s2, size_t __n);
int strcmp(const char *__s1, const char *__s2);
int strcoll(const char *__s1, const char *__s2);
int strncmp(const char *__s1, const char *__s2, size_t __n);
size_t strxfrm(char *__restrict __s1, const char *__restrict __s2, size_t __n);
void *memchr(const void *__s, int __c, size_t __n);
char *strchr(const char *__s, int __c);
size_t strcspn(const char *__s1, const char *__s2);
char *strpbrk(const char *__s1, const char *__s2);
char *strrchr(const char *__s, int __c);
size_t strspn(const char *__s1, const char *__s2);
char *strstr(const char *__s1, const char *__s2);
char *strtok(char *__restrict __s1, const char *__restrict __s2);
void *memset(void *__s, int __c, size_t __n);
char *strerror(int __errnum);
size_t strlen(const char *__s);

#ifdef _PROXIMA_SOURCE
char *strdup(const char *__s);
#endif /* defined(_PROXIMA_SOURCE)*/

#ifdef __PLIBC_BUILDING_PLIBC
#define memchr __builtin_memchr
#define memcmp __builtin_memcmp
#define memcpy __builtin_memcpy
#define memmove __builtin_memmove
#define memset __builtin_memset
#define strcat __builtin_strcat
#define strchr __builtin_strchr
#define strcmp __builtin_strcmp
#define strcpy __builtin_strcpy
#define strlen __builtin_strlen
#define strncat __builtin_strncat
#define strncmp __builtin_strncmp
#define strncpy __builtin_strncpy
#endif

#ifdef __cplusplus
};
#endif

#endif /* _STRING_H */
