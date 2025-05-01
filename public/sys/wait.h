#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "sys/wait.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define WEXITSTATUS(stat_val) ((stat_val) & 0xff)
#define WTERMSIG(stat_val) (((stat_val) >> 8) & 0x7f)
#define WSTOPSIG(stat_val) WEXITSTATUS(stat_val)

#define WIFEXITED(stat_val) (WTERMSIG(stat_val) == 0)
#define WIFSIGNALED(stat_val) (((WTERMSIG(stat_val) + 1) & 0x7f) > 1)
#define WIFSTOPPED(stat_val) (WTERMSIG(stat_val) == 0x7f)

#define WNOHANG (1 << 0)
#define WUNTRACED (1 << 1)

__pid_t wait(int *__stat_loc);
__pid_t waitpid(__pid_t __pid, int *__stat_loc, int __options);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_WAIT_H */
