/*
 * kernel_super/signals/sigaction.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/04
 * Author: 	Lukasz Dembinski
 * Info:	sigaction.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>
#include <agnix/tasks.h>
#include <agnix/signals.h>
#include <agnix/spinlock.h>
#include <agnix/memory.h>
#include <agnix/errno.h>

int do_sys_sigaction(int sig, struct sigaction_s *act, struct sigaction_s *oldact)
{
    struct task_s *task = current_task;

    if (act == NULL && oldact == NULL)
	return -EINVAL;

    spin_lock_irq(&(task->signals_act.lock));
    
    if (oldact != NULL)
        memcpy(oldact, &(task->signals_act.sigaction[sig]), sizeof(struct sigaction_s));

    if (act != NULL)
        memcpy(&(task->signals_act.sigaction[sig]), act, sizeof(struct sigaction_s));

    spin_unlock_irq(&(task->signals_act.lock));

    return 0;
}


