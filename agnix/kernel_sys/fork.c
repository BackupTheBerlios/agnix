/*
 * kernel_sys/fork.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	fork.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <asm/types.h>
#include <agnix/sched.h>
#include <asm/tss.h>
#include <agnix/memory.h>
#include <agnix/tasks.h>
#include <agnix/list.h>
#include <agnix/console.h>
#include <agnix/hashing.h>
#include <agnix/spinlock.h>
#include <agnix/errno.h>

//extern struct tss_wrap_s *task_fork(struct regs_s *, u32 esp);

extern struct list_head running_tasks_list;
extern spinlock_t running_list_lock;

int fork_hashd;
static int last_pid = 0;

int fork_add_hash_entry(int pid, struct task_s *task) 
{
    return hash_entry_add(fork_hashd, (u32)pid, (u32)task);
}

struct task_s *fork_find_hash_entry(int pid) 
{
    u32 task_ptr;
    
    task_ptr = hash_entry_find(fork_hashd, (u32)pid);
    
    return (struct task_s *)task_ptr;
}

int get_free_pid(void)
{
    return ++last_pid;
}

void put_free_pid(int pid)
{
    return;
}

int do_sys_fork(struct regs_s *regs)
{
    struct task_s *cur_task = current_task;
    struct task_s *new_task;
    struct tss_wrap_s *new_tss_wrap;
    int pid;
    int ret;
    u32 flags;

    new_task = (struct task_s *)get_free_pages(1);

    ret = -EAGAIN;
    if ((pid = get_free_pid()) < 0)
	goto do_sys_fork_error;
	
    new_task->t_pid = pid;

    ret = -ENOMEM;
    if ((new_tss_wrap = task_fork(regs, (u32)new_task + 0x1000)) == 0)
	goto do_sys_fork_error;
	
    new_task->tss_wrap 	= new_tss_wrap;    
    new_task->t_state 	= TASK_STAT_STARTING;
    new_tss_wrap->task 	= new_task;

    if (signals_fork(cur_task, new_task) != 0)
   	goto do_sys_fork_error;

    if (fork_add_hash_entry(pid, new_task) == -1) {
	kernel_panic("Cannot add task's pid to hash table!\n");
	return -1;
    }

    INIT_LIST_HEAD(&(new_task->task_list));
    
    spin_lock_irqsave(&running_list_lock, flags);
//    list_add(&(new_task->task_list), &running_tasks_list);
    sched_activate_task_lock(new_task);
    spin_unlock_irqrestore(&running_list_lock, flags);

    ret = 0;
    
do_sys_fork_error:

    return ret;
}

int do_task_exit(struct task_s *task, int exit_code)
{
    u32 flags;

    spin_lock_irqsave(&running_list_lock, flags);
    list_del(&(task->task_list));
    spin_unlock_irqrestore(&running_list_lock, flags);

    put_free_pid(task->t_pid);

    task_exit(task->tss_wrap);

    return 0;
}

int do_sys_exit(int exit_code)
{
    do_task_exit(current_task, exit_code);

    return 0;
}

int fork_init(void)
{
    if ((fork_hashd = register_hash_table(1024, NULL)) < 0) {
	kernel_panic("Can not create hash table for fork!\n");
	
	return -1;
    }

    fork_add_hash_entry(0, current_task);

    return 0;
}
