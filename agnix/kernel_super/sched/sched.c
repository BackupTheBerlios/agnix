/*
 * kernel_super/sched/sched.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	sched.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/list.h>
#include <agnix/queues.h>
#include <agnix/tasks.h> 
#include <agnix/math64.h>
#include <agnix/console.h>
#include <agnix/memory.h>
#include <agnix/spinlock.h>
#include <asm/irq.h>

struct list_head running_tasks_list;
struct list_head ready_tasks_list;
struct list_head stopped_tasks_list;

spinlock_t running_list_lock;
spinlock_t ready_list_lock;
spinlock_t stopped_list_lock;

extern struct task_s *idle_task;

void sched_queue_task(void)
{
}

void sched_activate_task_lock(struct task_s *task)
{
    if (!(task->t_state & TASK_STAT_RUNNING)) {	
	task->t_state |= TASK_STAT_RUNNING;
	list_add(&(task->task_list), &running_tasks_list);
    }
}

void sched_activate_task(struct task_s *task)
{
    u32 flags;

    spin_lock_irqsave(&running_list_lock, flags);
    sched_activate_task_lock(task);
    spin_unlock_irqrestore(&running_list_lock, flags);
}

void sched_deactivate_task_lock(struct task_s *task)
{
    if (task->t_state & TASK_STAT_RUNNING) {	
	task->t_state &= ~TASK_STAT_RUNNING;
    }
}

void sched_deactivate_task(struct task_s *task)
{
    u32 flags;

    spin_lock_irqsave(&running_list_lock, flags);
    sched_deactivate_task_lock(task);
    spin_unlock_irqrestore(&running_list_lock, flags);
}

/* we have only Round Robin schedule algorithm */
void reschedule_task(struct task_s *task)
{
    list_del(&(task->task_list));
    list_add_tail(&(task->task_list), &running_tasks_list);
}

void schedule_task(void)
{
    struct task_s *prev, *next;
    struct list_head *head;
    u16 prev_tss_entry;
    u16 next_tss_entry;
    u32 flags;
    int prev_pid;

    spin_lock_irqsave(&running_list_lock, flags);

    prev = current_task;

    if ((prev->t_state & TASK_STAT_KILLED) || (!(prev->t_state & TASK_STAT_RUNNING))) {
	if (prev->t_pid) {
	    list_del(&prev->task_list);
	}
    }

    if (list_empty(&running_tasks_list)) {
	next = idle_task;
    } else {
        head = running_tasks_list.next;
	next = list_entry(head, struct task_s, task_list);
    }

    next->t_state &= ~TASK_STAT_STARTING;
    prev_tss_entry = prev->tss_wrap->tss_entry;
    next_tss_entry = next->tss_wrap->tss_entry;
    prev_pid = prev->t_pid;

    prev->t_state &= ~TASK_STAT_RESCHED;
    prev->t_count = INIT_COUNT;

//    if (!((prev->t_state & TASK_STAT_KILLED) && (!(prev->t_state & TASK_STAT_RUNNING)))) {
//        prev->t_state &= ~TASK_STAT_RESCHED;
//	prev->t_count = INIT_COUNT;
//    } else
//    if (!(prev->t_state & TASK_STAT_RUNNING)) {
//        spin_lock(&stopped_list_lock);
//	list_add(&prev->task_list, &stopped_tasks_list);    
//	spin_unlock(&stopped_list_lock);
//    } else 
    if (prev->t_state & TASK_STAT_KILLED) {
	/* narazie tak. to musi byc zrobione przez sys_wait() */
    	put_free_pages((u32)prev, 1);
    }
    
    if (next != idle_task)
        reschedule_task(next);

    if (prev_pid != next->t_pid)
        task_switch(next_tss_entry);    
	
    spin_unlock_irqrestore(&running_list_lock, flags);
}

int __init scheduler_init(void)
{
    INIT_LIST_HEAD(&running_tasks_list);
    INIT_LIST_HEAD(&ready_tasks_list);
    INIT_LIST_HEAD(&stopped_tasks_list);

    spin_lock_init(&running_list_lock);
    spin_lock_init(&ready_list_lock);
    spin_lock_init(&stopped_list_lock);

    return 0;
}
