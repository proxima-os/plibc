#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H 1

#include <bits/__pid_t.h>
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "sys/wait.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define WNOHANG (1 << 0)
#define WUNTRACED (1 << 1)

/*
 * Encoding of stat_val:
 * if exited normally:
 *  bits 0..7 = low eight bits of exit status
 * if signaled:
 *  bits 0..6 = signal number
 *  bit 7 = 0 if terminated, 1 if stopped
 * bit 8 = 0 if exited normally, 1 if signaled
 */

#define WIFEXITED(stat_val) (!((stat_val) & 0x100))
#define WIFSIGNALED(stat_val) (((stat_val) & 0x180) == 0x100)
#define WIFSTOPPED(stat_val) (((stat_val) & 0x180) == 0x180)

#define WEXITSTATUS(stat_val) ((stat_val) & 0xff)
#define WTERMSIG(stat_val) ((stat_val) & 0x7f)
#define WSTOPSIG(stat_val) WTERMSIG(stat_val)

__pid_t wait(int *__stat_loc);
__pid_t waitpid(__pid_t __pid, int *__stat_loc, int __options);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_WAIT_H */
