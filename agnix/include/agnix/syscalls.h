#ifndef _AGNIX_SYSCALLS_H_
#define _AGNIX_SYSCALLS_H_

#include <agnix/tasks.h>
#include <asm/types.h>
#include <asm/tss.h>

extern int do_sys_fork(struct regs_s *regs);
extern int do_sys_exit(int exit_code);
extern int do_sys_nice(int nice);
extern int do_sys_getpid(void);
extern int do_sys_reboot(void);
extern int do_sys_kill(int pid, int sig);
extern int do_sys_sigaction(int sig, struct sigaction_s *act, struct sigaction_s *oldact);
extern int do_sys_sigreturn(unsigned long _unused);
extern int do_sys_setitimer(int which, struct itimerval_s *newval, struct itimerval_s *oldval);
extern int do_sys_getitimer(int which, struct itimerval_s *val);
extern int do_sys_time(int *secs);
extern int do_sys_stime(int *secs);

#define SYS_CALL_NAME(name) sys_##name
#define SYS_CALL(name) int SYS_CALL_NAME(name)(struct regs_s *regs)

SYS_CALL(fork);
SYS_CALL(exit);
SYS_CALL(nice);
SYS_CALL(getpid);
SYS_CALL(kill);
SYS_CALL(gettimeofday);
SYS_CALL(settimeofday);
SYS_CALL(setuid);
SYS_CALL(getuid);
SYS_CALL(setgid);
SYS_CALL(getgid);
SYS_CALL(sigaction);
SYS_CALL(sigreturn);
SYS_CALL(setitimer);
SYS_CALL(getitimer);
SYS_CALL(time);
SYS_CALL(stime);
SYS_CALL(not_implemented);

#endif
