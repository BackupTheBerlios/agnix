/*
 * kernel_sys/sys_unix.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	sys_unix.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/tasks.h>
#include <agnix/timer.h>
#include <agnix/errno.h>

int do_sys_nice(int nice)
{
    if (current_task->t_uid != 0) {
	if (current_task->t_nice >= nice)
	    return -EPERM;
    }
    
    current_task->t_nice = nice;
    
    return 0;
}

int do_sys_getpid(void)
{
    return current_task->t_pid;
}
