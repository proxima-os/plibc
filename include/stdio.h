#ifndef _STDIO_H
#define _STDIO_H 1

#include <bits/FILE.h>
#include <bits/__off_t.h>
#include <bits/features.h>
#include <bits/whence.h> /* IWYU pragma: export */

#define __need___va_list
#include <stdarg.h> /* IWYU pragma: export */

#define __need_size_t
#define __need_NULL
#include <stddef.h> /* IWYU pragma: export */

#if _POSIX_C_SOURCE >= 200809L
#include <bits/ssize_t.h> /* IWYU pragma: export */
#endif                    /* _POSIX_C_SOURCE >= 200809L */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __off_t __off;
} fpos_t;

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define EOF (-1)

#define BUFSIZ 16384
#define FOPEN_MAX 1024
#define FILENAME_MAX 16

#define L_tmpnam 20      /* /tmp/tmp.XXXXXXXXXX is 20 chars (inc. null terminator) */
#define TMP_MAX 0x200000 /* the real value is much larger than this, but the constant's just a minimum, so whatever */

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

/* C90 says these are macros */
#define stdin stdin
#define stdout stdout
#define stderr stderr

int remove(const char *__filename);
int rename(const char *__old, const char *__new);
FILE *tmpfile(void);
char *tmpnam(char *__s);
int fclose(FILE *__stream);
int fflush(FILE *__stream);
FILE *fopen(const char *__restrict __filename, const char *__restrict __mode);
FILE *freopen(const char *__restrict __filename, const char *__restrict __mode, FILE *__restrict __stream);
void setbuf(FILE *__restrict __stream, char *__restrict __buf);
int setvbuf(FILE *__restrict __stream, char *__restrict __buf, int __mode, size_t __size);
int fprintf(FILE *__restrict __stream, const char *__restrict __format, ...);
int fscanf(FILE *__restrict __stream, const char *__restrict __format, ...);
int printf(const char *__restrict __format, ...);
int scanf(const char *__restrict __format, ...);
int sprintf(char *__restrict __s, const char *__restrict __format, ...);
int sscanf(const char *__restrict __s, const char *__restrict __format, ...);
int vfprintf(FILE *__restrict __stream, const char *__restrict __format, __gnuc_va_list __arg);
int vprintf(const char *__restrict __format, __gnuc_va_list __arg);
int vsprintf(char *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg);
int fgetc(FILE *__stream);
char *fgets(char *__restrict __s, int __n, FILE *__restrict __stream);
int fputc(int __c, FILE *__stream);
int fputs(const char *__restrict __s, FILE *__restrict __stream);
int getc(FILE *__stream);
int getchar(void);
char *gets(char *__s);
int putc(int __c, FILE *__stream);
int putchar(int __c);
int puts(const char *__s);
int ungetc(int __c, FILE *__stream);
size_t fread(void *__restrict __ptr, size_t __size, size_t __nmemb, FILE *__restrict __stream);
size_t fwrite(const void *__restrict __ptr, size_t __size, size_t __nmemb, FILE *__restrict __stream);
int fgetpos(FILE *__restrict __stream, fpos_t *__restrict __pos);
int fseek(FILE *__stream, long __offset, int __whence);
int fsetpos(FILE *__restrict __stream, const fpos_t *__restrict __pos);
long ftell(FILE *__stream);
void rewind(FILE *__stream);
void clearerr(FILE *__stream);
int feof(FILE *__stream);
int ferror(FILE *__stream);
void perror(const char *__s);

#ifdef _POSIX_C_SOURCE

#define L_ctermid 9 /* /dev/tty is 9 chars (inc. null terminator) */

char *ctermid(char *__s);
int fileno(FILE *__stream);
FILE *fdopen(int __fildes, const char *__type);

#endif /* defined(_POSIX_C_SOURCE) */

#ifdef _ISOC99_SOURCE

int vsnprintf(char *__restrict __s, size_t __n, const char *__restrict __format, __gnuc_va_list __arg);
int snprintf(char *__restrict __s, size_t __n, const char *__restrict __format, ...);

#endif /* defined(_ISOC99_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _STDIO_H */
