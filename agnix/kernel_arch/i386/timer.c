/*
 * kernel_arch/i386/kernel/timer.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	timer.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/irq.h>
#include <agnix/tasks.h>
#include <agnix/timer.h>
#include <agnix/memory.h>
#include <agnix/adi/adi.h>
#include <agnix/spinlock.h>
#include <agnix/timers.h>
#include <agnix/counters.h>
#include <agnix/console.h>

#define TIMER_INC_VAL	(1e6 / TIMER_HZ)	/* usec */
#define USEC_IN_SEC	1000000

struct timeval_s current_time;
extern struct chip_s rtc_chip;
extern struct chip_s i8254_chip;
extern int counter_jiffies_desc;

spinlock_t current_time_lock;

void timer_usec_overflow(void)
{
    do {
	current_time.tv_usec -= USEC_IN_SEC;
	current_time.tv_sec++;
    } while (current_time.tv_usec > USEC_IN_SEC);
}

void timer_actualize(void)
{
    current_time.tv_usec += TIMER_INC_VAL;
    
    if (current_time.tv_usec > USEC_IN_SEC)
	timer_usec_overflow();
}

void timer_irq(u32 i)
{
    if (--current_task->t_count <= 0) {
	current_task->t_state |= TASK_STAT_RESCHED;
    }
    else
	current_task->t_state &= ~TASK_STAT_RESCHED;
    
    timer_actualize();
    counter_inc(counter_jiffies_desc);
    run_timers();
}

struct irq_routine_s irq_timer = { timer_irq, 0, 0, };

int __init timer_init(void)
{
    spin_lock_init(&current_time_lock);

    rtc_ops(&rtc_chip)->gettime(&rtc_chip, &current_time);
    install_irq(0, &irq_timer);

    return 0;
}

int timer_gettimeofday(struct timeval_s *tv)
{
    u16 count;
    u32 flags;

    spin_lock_irqsave(&current_time_lock, flags);
    
    memcpy(tv, &current_time, sizeof(struct timeval_s));
    count = pit_ops(&i8254_chip)->read_count(&i8254_chip, 0);
    tv->tv_usec += (int)(count);
    
    spin_unlock_irqrestore(&current_time_lock, flags);

    return 0;
}

int timer_settimeofday(struct timeval_s *tv)
{
    u16 count;
    u32 flags;

    spin_lock_irqsave(&current_time_lock, flags);
    
    memcpy(&current_time, tv, sizeof(struct timeval_s));
    count = (u16)(tv->tv_usec % 1000);
    pit_ops(&i8254_chip)->write_count(&i8254_chip, 0, count);
    
    spin_unlock_irqrestore(&current_time_lock, flags);

    return 0;
}
