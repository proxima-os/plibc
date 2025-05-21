#ifndef _STDIO_H
#define _STDIO_H 1

#include <bits/NULL.h>
#include <bits/features.h>
#include <bits/seek.h> /* IWYU pragma: export */
#include <bits/size_t.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSIZ 4096
#define EOF (-1)
#define FOPEN_MAX 256
#define FILENAME_MAX 4096
#define L_tmpnam 20 /* /tmp/tmp.XXXXXXXXXX is 19 chars */
#define TMP_MAX 0x200000

#define stderr stderr
#define stdin stdin
#define stdout stdout

typedef struct _FILE {
    struct _FILE *__prev;
    struct _FILE *__next;
    void *__buffer;
    void *__buf_start;
    void *__buf_cur;
    void *__buf_end;
    int __fd;
    unsigned __unget;
    unsigned __nunget;
    unsigned __eof : 1; /* end of file indicator */
    unsigned __err : 1; /* error indicator */
    unsigned __own : 1; /* is the buffer managed by libc? */
    unsigned __lbf : 1; /* is the stream line-buffered? */
    unsigned __rbf : 1; /* does the buffer contain read data? */
    unsigned __fd_read : 1;
    unsigned __fd_write : 1;
} FILE;

typedef struct {
    __off_t __pos;
} fpos_t;

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

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
__attribute__((__format__(__printf__, 2, 3))) int fprintf(
        FILE *__restrict __stream,
        const char *__restrict __format,
        ...
);
__attribute__((__format__(__scanf__, 2, 3))) int fscanf(
        FILE *__restrict __stream,
        const char *__restrict __format,
        ...
);
__attribute__((__format__(__printf__, 1, 2))) int printf(const char *__restrict __format, ...);
__attribute__((__format__(__scanf__, 1, 2))) int scanf(const char *__restrict __format, ...);
__attribute__((__format__(__printf__, 2, 3))) int sprintf(char *__restrict __s, const char *__restrict __format, ...);
__attribute__((__format__(__scanf__, 2, 3))) int sscanf(
        const char *__restrict __s,
        const char *__restrict __format,
        ...
);
__attribute__((__format__(__printf__, 2, 0))) int vfprintf(
        FILE *__restrict __stream,
        const char *__restrict __format,
        __va_list __arg
);
__attribute__((__format__(__printf__, 1, 0))) int vprintf(const char *__restrict __format, __va_list __arg);
__attribute__((__format__(__printf__, 2, 0))) int vsprintf(
        char *__restrict __s,
        const char *__restrict __format,
        __va_list __arg
);
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

#if _POSIX_C_SOURCE >= 1

#define L_ctermid 9 /* /dev/tty is 8 chars, plus 1 for the null terminator */

char *ctermid(char *__s);
FILE *fdopen(int __fildes, const char *__type);
int fileno(FILE *__stream);

#endif /* _POSIX_C_SOURCE >= 1 */

#ifdef _PROXIMA_SOURCE

__attribute__((__format__(__printf__, 3, 0))) int vsnprintf(
        char *__restrict __s,
        size_t __n,
        const char *__restrict __format,
        __va_list arg
);
__attribute__((__format__(__printf__, 3, 4))) int snprintf(
        char *__restrict __s,
        size_t __n,
        const char *__restrict __format,
        ...
);

__attribute__((__format__(__scanf__, 2, 0))) int vfscanf(
        FILE *__restrict __stream,
        const char *__restrict __format,
        __va_list __arg
);
__attribute__((__format__(__scanf__, 1, 0))) int vscanf(const char *__restrict __format, __va_list arg);
__attribute__((__format__(__scanf__, 2, 0))) int vsscanf(
        const char *__restrict __s,
        const char *__restrict __format,
        __va_list arg
);

#endif /* defined(_PROXIMA_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _STDIO_H */
