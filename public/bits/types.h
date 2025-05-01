#ifndef _BITS_TYPES_H
#define _BITS_TYPES_H

typedef __INT8_TYPE__ __int8_t;
typedef __INT16_TYPE__ __int16_t;
typedef __INT32_TYPE__ __int32_t;
typedef __INT64_TYPE__ __int64_t;
typedef __UINT8_TYPE__ __uint8_t;
typedef __UINT16_TYPE__ __uint16_t;
typedef __UINT32_TYPE__ __uint32_t;
typedef __UINT64_TYPE__ __uint64_t;

typedef __SIZE_TYPE__ __size_t;
typedef __builtin_va_list __va_list;
typedef __WCHAR_TYPE__ __wchar_t;

typedef __int64_t __clock_t;
typedef __uint64_t __dev_t;
typedef __uint32_t __gid_t;
typedef __uint64_t __ino_t;
typedef __uint32_t __mode_t;
typedef __uint64_t __nlink_t;
typedef __int64_t __off_t;
typedef __int32_t __pid_t;
typedef long __ssize_t;
typedef __int64_t __time_t;
typedef __uint32_t __uid_t;

typedef __uint64_t __sigset_t;

struct __timespec {
    __time_t tv_sec;
    long tv_nsec;
};

#endif /* _BITS_TYPES_H */
