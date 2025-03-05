#ifndef _BITS_FILE_H
#define _BITS_FILE_H 1

typedef struct _FILE {
    int __fd;
    int __fd_read : 1;
    int __fd_write : 1;
    int __flushnl : 1;
    int __buf_owned : 1;
    int __buf_write : 1;
    int __eof : 1;
    int __err : 1;
    void *__buf_start;
    void *__buf_cur;
    void *__buf_end;
    struct _FILE *__prev;
    struct _FILE *__next;
    unsigned char __unget_buf[12];
    int __unget_idx;
} FILE;

#endif /* _BITS_FILE_H */
