/*
 * kernel_super/timers/itimers.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/08
 * Author: 	Lukasz Dembinski
 * Info:	itimers.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/tasks.h>
#include <agnix/timer.h>
#include <agnix/timers.h>
#include <agnix/math64.h>
#include <agnix/signals.h>
#include <agnix/errno.h>
#include <agnix/console.h>

#define MOD_NAME			"iTIMERS: "

int timeval_to_jiffies(struct timeval_s *val, u32 *jiffies_buf)
{
    u32 jiffies_usecs;
    
    mul_32_32(jiffies_buf, (u32)(val->tv_sec), TIMER_HZ);
    jiffies_usecs = (u32)(val->tv_usec) / TIMER_HZ;
    add_64_32(jiffies_buf, jiffies_buf, jiffies_usecs);

    return 0;
}

void itimer_expire(void *data)
{
    struct task_s *task = (struct task_s *)data;
    struct timer_s *timer = &task->timer_real;

    signal_send(task->t_pid, SIGALRM, (siginfo_t *)1);

    if (task->timer_real_interval) {
        timer->timer_expire = task->timer_real_interval;
//	printk("itimer interval %d\n", task->timer_real_interval);
    }
    
//    printk("itimer expired\n");
}

int do_sys_setitimer(int which, struct itimerval_s *newval, struct itimerval_s *oldval)
{
    struct timer_s *timer = &current_task->timer_real;
    u32 jiffies_interval[2];
    u32 jiffies_value[2];

    /* yes, we have to do user address checking */
    
    timeval_to_jiffies(&newval->it_value, jiffies_value);
    timeval_to_jiffies(&newval->it_interval, jiffies_interval);

    if (jiffies_value[1] || jiffies_interval[1])
	return -EINVAL;

//    memcpy(&current_task->timeval_real, newval, sizeof(struct itimerval_s));
//    printk(MOD_NAME "added timer %d %d\n", jiffies_value[0], jiffies_interval[0]);

    current_task->timer_real_expire   = jiffies_value[0];
    current_task->timer_real_interval = jiffies_interval[0];
    timer->timer_function 	= itimer_expire;
    timer->timer_data 		= (void *)current_task;
    timer->timer_expire 	= jiffies_value[0];
    
    if (!current_task->timer_real_interval)
	timer->timer_flags	= TIMER_FLAG_ONESHOT;
    
    register_timer(timer);
    
    return 0;
}

int do_sys_getitimer(int which, struct itimerval_s *val)
{
    return 0;
}
