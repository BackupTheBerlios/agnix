/*
 * kernel_super/timers/timers.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/08
 * Author: 	Lukasz Dembinski
 * Info:	timers.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/timers.h>
#include <asm/bitops.h>
#include <agnix/spinlock.h>
#include <agnix/queues.h>
#include <agnix/list.h>
#include <agnix/console.h>

#define MOD_NAME			"TIMERS: "

#define TIMER_SET_EXP_BIN(bits)	 	timer_set_exp_bin(bits)
#define TIMER_SET_EXP(bits)	 	(TIMER_SET_EXP_BIN(bits) * TIMER_MAX_SET_ENTRIES)
#define TIMER_SET_RATE(bits)	 	((TIMER_SET_EXP(bits + 1) - TIMER_SET_EXP(bits)) / TIMER_MAX_SET_ENTRIES)

int 			timers_initialized = 0;
spinlock_t	 	timer_lock;
struct timer_set_s 	timer_sets[TIMER_MAX_SETS];

unsigned int timer_set_exp_bin(unsigned long x)
{
    if (x - 1 < 0)
	return 0;

    return (1 << (x - 1));
}

int timer_add(struct timer_s *timer, u32 timer_set, u32 timer_idx)
{
    list_add(&timer->timer_list, &timer_sets[timer_set].entries[timer_idx].entry_list);
    set_bit(timer_idx, timer_sets[timer_set].entries_pending);
    timer_sets[timer_set].entries[timer_idx].entry_count++;
    timer->timer_set = timer_set;
    timer->timer_idx = timer_idx;
    
    return 0;
}

int timer_del(struct timer_s *timer)
{
    u32 timer_set = timer->timer_set;
    u32 timer_idx = timer->timer_idx;
    
    list_del(&timer->timer_list);
    
    if (list_empty(&timer_sets[timer_set].entries[timer_idx].entry_list))
        clear_bit(timer_idx, timer_sets[timer_set].entries_pending);

    timer_sets[timer_set].entries[timer_idx].entry_count--;
    
    return 0;
}

int register_timer_lock(struct timer_s *timer)
{
    u32 timer_idx;
    u32 timer_set;
    u32 i;

    for (i = 0; i < TIMER_MAX_SET_ENTRIES; i++) {
	if (timer_sets[i].expire_min <= timer->timer_expire &&
	    timer_sets[i].expire_max >= timer->timer_expire) {
		timer_set = i;
		timer_idx = (timer->timer_expire - timer_sets[i].expire_min) /
			     timer_sets[i].expire_rate;
			     
		timer_idx = (timer_idx + timer_sets[i].idx_head) % (TIMER_MAX_SET_ENTRIES - 1);
		timer_add(timer, timer_set, timer_idx);

		break;
	}
    }    

    return 0;
}

int register_timer(struct timer_s *timer)
{
    int ret;

    spin_lock_irq(&timer_lock);
    ret = register_timer_lock(timer);
    spin_unlock_irq(&timer_lock);
    
    return ret;
}

int modify_timer(struct timer_s *timer) 
{
    return 0;
}

int unregister_timer_lock(struct timer_s *timer)
{
    return timer_del(timer);
}

int unregister_timer(struct timer_s *timer)
{
    int ret;
    
    spin_lock_irq(&timer_lock);
    ret = unregister_timer_lock(timer);
    spin_unlock_irq(&timer_lock);

    return ret;
}

int run_timers_expired(void)
{
    struct timer_s *timer;
    struct list_head *tmp, *tmp_2;
    u32 timer_idx;
    
    timer_idx = timer_sets[0].idx_head;
    
    if (test_and_clear_bit(timer_idx, timer_sets[0].entries_pending)) {	
	list_for_each_safe(tmp, tmp_2, &timer_sets[0].entries[timer_idx].entry_list) {
	    timer = list_entry(tmp, struct timer_s, timer_list);

	    unregister_timer_lock(timer);

	    spin_unlock_irq(&timer_lock);
	    timer->timer_function(timer->timer_data);
	    spin_lock_irq(&timer_lock);

	    if (!(timer->timer_flags & TIMER_FLAG_ONESHOT))
		register_timer_lock(timer);
	}
    }

    timer_sets[0].idx_head = (timer_idx + 1) % (TIMER_MAX_SET_ENTRIES - 1);

    return 0;
}

int run_timers_far(void)
{
    int i;
    u32 idx_head;
    u32 idx_tail_prev;
    u32 idx_offset;
    u32 idx_offset_back;
    u32 expire_rate_prev;
    struct timer_s *timer;
    struct list_head *entry_list;
    struct list_head *tmp, *tmp_2;

    for (i = 1; i < TIMER_MAX_SETS; i++) {
	
	if ((timer_sets[i].expire_rate > 1) && (timer_sets[0].idx_head % timer_sets[i].expire_rate))
	    break;

	idx_head = timer_sets[i].idx_head;
	
	if (test_and_clear_bit(idx_head, timer_sets[i].entries_pending)) {
	    expire_rate_prev = timer_sets[i - 1].expire_rate;
	    entry_list 	     = &(timer_sets[i].entries[idx_head].entry_list);
	    idx_tail_prev    = (timer_sets[i - 1].idx_head - 1) % (TIMER_MAX_SET_ENTRIES - 1);

	    idx_offset_back = 0;
	    list_for_each_safe(tmp, tmp_2, entry_list) {
		timer = list_entry(tmp, struct timer_s, timer_list);

		if (i > 1)
		    idx_offset_back = ((timer->timer_expire / expire_rate_prev) + 1) % 2;
		
		idx_offset = (idx_tail_prev - idx_offset_back) % (TIMER_MAX_SET_ENTRIES - 1);
		timer_add(timer, i - 1, idx_offset);
	    }
	    
	    INIT_LIST_HEAD(entry_list);
	    timer_sets[i].entries[idx_head].entry_count = 0;
	}
	
	timer_sets[i].idx_head = (timer_sets[i].idx_head + 1) % (TIMER_MAX_SET_ENTRIES - 1);
    }

    return 0;
}

int run_timers(void)
{
    if (!timers_initialized)
	return 0;
    
    spin_lock_irq(&timer_lock);
    run_timers_expired();
    run_timers_far();
    spin_unlock_irq(&timer_lock);

    return 0;
}

int timers_init(void)
{
    int i, j;

    printk(MOD_NAME "Initializing timers\n");

    for (i = 0; i < TIMER_MAX_SETS; i++) {

	timer_sets[i].expire_min  = TIMER_SET_EXP(i);
	timer_sets[i].expire_rate = TIMER_SET_RATE(i);
	timer_sets[i].expire_max  = timer_sets[i].expire_min + (TIMER_MAX_SET_ENTRIES - 1) * timer_sets[i].expire_rate;
	timer_sets[i].idx_head = 0;

	for (j = 0; j < TIMER_MAX_SET_ENTRIES; j++) {
	    INIT_LIST_HEAD(&timer_sets[i].entries[j].entry_list);
	    timer_sets[i].entries[j].entry_count = 0;
	}
    }
        
    spin_lock_init(&timer_lock);
    timers_initialized = 1;
    
    return 0;
}
