/*
 * kernel_super/signals/signals.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	signals.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>
#include <agnix/tasks.h>
#include <agnix/signals.h>
#include <agnix/memory.h>
#include <agnix/errno.h>
#include <agnix/list.h>
#include <asm/bitops.h>
#include <agnix/console.h>

#define MOD_NAME	"SIG: "

void signals_init(struct task_s *parent, struct task_s *task)
{
    int i;
    
    for (i = 0; i < NSIG; i++) {
	INIT_LIST_HEAD(&task->signals.list[i]);
    }
    
    task->signals.pending = 0;
    memset(task->signals.sigset.sig, 0, sizeof(sigset_t));
    memcpy(task->signals.sigmask.sig, parent->signals.sigmask.sig, sizeof(sigset_t));
}

int signals_sigaction_copy(struct task_s *parent, struct task_s *task)
{
    return 0;
}

int signals_fork(struct task_s *parent, struct task_s *task)
{
    signals_init(parent, task);    
    signals_sigaction_copy(parent, task);

    return 0;
}

void signal_remove(struct task_s *task, int sig)
{
    struct signals_s *signals;
    struct signal_s *signal;
    struct list_head *tmp;

    signals = &task->signals;

    spin_lock(&signals->lock);
    
    if (list_empty(&signals->list[sig]))
	return;
    
    list_for_each(tmp, &signals->list[sig]) {
	signal = list_entry(tmp, struct signal_s, list);
	
	put_free_pages((u32)signal, 0);
    }
    
    INIT_LIST_HEAD(&signals->list[sig]);
    
    spin_unlock(&signals->lock);
}

int signal_exclude(struct task_s *task, struct signals_s *signals, int sig)
{
    switch(sig) {
	case SIGCONT: 
	    signal_remove(task, SIGSTOP);
	    break;
	case SIGSTOP:
	    signal_remove(task, SIGCONT);
	    break;
    }
    
    return 0;
}

int signal_check(struct signals_s *signals, int sig)
{
    if (sig < 0 || sig > NSIG)
	return -1;

    /* sanity check */
    if (signals == NULL)
	return -1;

    return 0;
}

int signal_queue(struct signals_s *signals, int sig, struct signal_s *signal, 
		 siginfo_t *siginfo)
{
    signal->signal_nr = sig;
    memcpy(&signal->siginfo, siginfo, sizeof(siginfo_t));
    
    list_add(&signal->list, &signals->list[sig]);
    set_bit(sig, &signals->sigset.sig[0]);
    
    if (!test_bit(sig, &signals->sigmask.sig[0]))
	signals->pending++;

    return 0;
}

struct signal_s *signal_dequeue(struct task_s *task)
{
    struct signals_s *signals;
    struct signal_s *signal;
    sigset_t mask;
    int sig;
    u32 flags;

    signals = &task->signals;

    spin_lock_irqsave(&signals->lock, flags);

    mask.sig[0] = signals->sigset.sig[0] & ~signals->sigmask.sig[0];
    mask.sig[1] = signals->sigset.sig[1] & ~signals->sigmask.sig[1];

    if (mask.sig[0] == 0 && mask.sig[1] == 0)
	return NULL;

    sig = ffz(~mask.sig[0]);

    if (sig < 0 || sig >= 32)
	return NULL;

    if (list_empty(&signals->list[sig])) {
	printk(MOD_NAME "There is something wrong!\n");
	return NULL;
    }

    signal = list_entry(signals->list[sig].next, struct signal_s, list);
    list_del(&signal->list);

    signals->pending--;
    spin_unlock_irqrestore(&signals->lock, flags);

    return signal;
}

int send_signal_to_task(struct task_s *task, int sig, siginfo_t *siginfo)
{
    struct signals_s *signals;
    struct signal_s *signal;
    int ret;
    u32 flags;

    /* this will be done by the slab allocator */
    signal = (struct signal_s *)get_free_pages(0);
    
    if (signal == NULL)
	return -ENOMEM;

    memset(signal, 0, sizeof(struct signal_s));
    spin_lock_irqsave(&signals->lock, flags);

    signals = &task->signals;

    if (siginfo)
	memcpy(&(signal->siginfo), siginfo, sizeof(siginfo_t));
    
    ret = signal_check(signals, sig);
    if (ret < 0)
	goto signal_out;
    
    ret = signal_exclude(task, signals, sig);
    if (ret < 0)
	goto signal_out;
    
    ret = 0;
    signal_queue(signals, sig, signal, siginfo);

signal_out:
    spin_unlock_irqrestore(&signals->lock, flags);

    return ret;
}

int send_signal_to_pid(int pid, int sig, siginfo_t *siginfo)
{
    struct task_s *task;
    
    if ((task = fork_find_hash_entry(pid)) == 0) {
	printk("Can not find a task (pid=%d) in the hashing table\n", pid);
	return -1;
    }

    return send_signal_to_task(task, sig, siginfo);
}

int do_sys_kill(int pid, int sig)
{
    siginfo_t siginfo;

    siginfo.si_signo = sig;
    siginfo.si_errno = 0;
    siginfo.si_code  = SI_USER;
    siginfo.si_pid   = current_task->t_pid;
    siginfo.si_uid   = current_task->t_uid;
    
    return send_signal_to_pid(pid, sig, &siginfo);
}

int signal_send(int pid, int sig, siginfo_t *siginfo)
{
    siginfo_t siginfo_local;

    switch((u32)siginfo) {
	case 0:
            siginfo_local.si_signo = sig;
	    siginfo_local.si_errno = 0;
	    siginfo_local.si_code  = SI_USER;
            siginfo_local.si_pid   = current_task->t_pid;
	    siginfo_local.si_uid   = current_task->t_uid;
	    break;

	case 1:
            siginfo_local.si_signo = sig;
	    siginfo_local.si_errno = 0;
	    siginfo_local.si_code  = SI_KERNEL;
	    siginfo_local.si_pid   = 0;
	    siginfo_local.si_uid   = 0;
	    break;
    
	default:
	    return send_signal_to_pid(pid, sig, siginfo);
    }
    
    return send_signal_to_pid(pid, sig, &siginfo_local);
}

int do_sys_sigreturn(unsigned long __unused)
{
    printk("sigreturn!!!\n");

    return 0;
}
