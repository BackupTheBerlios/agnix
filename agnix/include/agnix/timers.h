#ifndef _AGNIX_TIMERS_H_
#define _AGNIX_TIMERS_H_

#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <asm/types.h>

#define TIMER_MAX_SET_ENTRIES		256
#define TIMER_MAX_SETS			16

#define TIMER_FLAG_ONESHOT		0x01
#define TIMER_FLAG_EXCLUSIVE		0x02

#define TIMER_MAX_PRIO			0x10
#define TIMER_PRIO_EXCLUSIVE		0x00
#define TIMER_PRIO_RT			0x01
#define TIMER_PRIO_NORMAL		0x02

struct timer_s {
    u32 timer_expire;
    void (*timer_function)(void *data);
    void *timer_data;

    u32 timer_set;
    u32 timer_idx;
    
    int timer_owner;
    int timer_priority;
    int timer_flags;
    struct list_head timer_list;
};

struct timer_set_entry_s {
    struct list_head entry_list;
    int entry_count;
};

struct timer_set_s {
    u32 expire_min;
    u32 expire_max;
    u32 expire_rate;
    u32 idx_head;
    
    struct timer_set_entry_s entries[TIMER_MAX_SET_ENTRIES];
    u32 entries_pending[TIMER_MAX_SET_ENTRIES / sizeof(u32)];
};

int register_timer(struct timer_s *timer);
int unregister_timer(struct timer_s *timer);
int modify_timer(struct timer_s *timer);
int run_timers(void);

#endif
