/*
 * kernel_arch/i386/kernel/syscalls.c
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
#include <agnix/init.h>
#include <agnix/linkage.h>
#include <agnix/unistd.h>
#include <agnix/tasks.h>
#include <agnix/syscalls.h>
#include <agnix/console.h>

extern struct tss_wrap_s init_tss[MAX_TASKS];
extern struct tss_s *syscall_tss;

int (*syscall_table[0xFF])(struct regs_s *regs) = 
{
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(exit),		/* 1 */
    SYS_CALL_NAME(fork),		/* 2 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 10 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(time),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(getpid),		/* 20 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 30 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(nice),		/* 34 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(kill),		/* 37 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 40 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 50 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 60 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(sigaction),		/* 67 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 70 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 80 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 90 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 100 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(setitimer),
    SYS_CALL_NAME(getitimer),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),	/* 110 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(sigreturn),		/* 119 */
    SYS_CALL_NAME(not_implemented),	/* 120 */
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented),
    SYS_CALL_NAME(not_implemented)
};

asmlinkage void syscall_irq(struct regs_s regs)
{
    u32 function;
    int ret;
    
    function = regs.eax;

    if (syscall_table[function] != 0) {
	ret = syscall_table[function](&regs);
	regs.eax = (u32)ret;
    }
}
