/*
 * kernel_sys/syscalls.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	syscalls.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/syscalls.h>
#include <agnix/errno.h>

#define MOD_NAME	"SYSCALLS: "

SYS_CALL(fork) 
{
    return do_sys_fork(regs);
}

SYS_CALL(exit) 
{
    return do_sys_exit((int)(regs->ebx));
}

SYS_CALL(nice) 
{
    return do_sys_nice((int)(regs->ebx));
}

SYS_CALL(getpid) 
{
    return do_sys_getpid();
}

SYS_CALL(gettimeofday) 
{
    return timer_gettimeofday((struct timeval_s *)(regs->ebx));
}

SYS_CALL(settimeofday) 
{
    return timer_settimeofday((struct timeval_s *)(regs->ebx));
}

SYS_CALL(kill) 
{
    return do_sys_kill((int)(regs->ebx), (int)(regs->ecx));
}

SYS_CALL(setuid) 
{
    return 0;
}

SYS_CALL(getuid) 
{
    return 0;
}

SYS_CALL(setgid) 
{
    return 0;
}

SYS_CALL(getgid) 
{
    return 0;
}

SYS_CALL(sigaction) 
{
    return do_sys_sigaction((int)(regs->ebx), (struct sigaction_s *)(regs->ecx), 
			    (struct sigaction_s *)(regs->edx));
}

SYS_CALL(sigreturn) 
{
    return do_sys_sigreturn((int)(regs->ebx));
}

SYS_CALL(setitimer) 
{
    return do_sys_setitimer((int)(regs->ebx), (struct itimerval_s *)(regs->ecx),
			    (struct itimerval_s *)(regs->edx));
}

SYS_CALL(getitimer) 
{
    return do_sys_getitimer((int)(regs->ebx), (struct itimerval_s *)(regs->ecx));
}

SYS_CALL(time)
{
    return do_sys_time((int *)(regs->ebx));
}

SYS_CALL(stime)
{
    return do_sys_stime((int *)(regs->ebx));
}

SYS_CALL(not_implemented)
{
    return -ENOSYS;
}
