/*
 * kernel_super/counters/counters_core.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/09
 * Author: 	Lukasz Dembinski
 * Info:	counters_core.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/spinlock.h>
#include <agnix/math64.h>
#include <agnix/resources.h>
#include <agnix/console.h>
#include <agnix/counters.h>
#include <asm/atomic.h>

#define MOD_NAME		"CNT: "

#define COUNTER_LOCK(cnt)	u32 flags; do { spin_lock_irqsave(&cnt->cnt_lock, flags); } while(0)
#define COUNTER_UNLOCK(cnt)	do { spin_unlock_irqrestore(&cnt->cnt_lock, flags); } while(0)

int counters_resource_desc;

struct counter_s counters[MAX_COUNTERS];

u32 counters_bitmap[COUNTERS_BITMAP_LEN];

struct resource_s counters_resource = {
    .resource_name	= "counters",
    .resource_bitmap	= counters_bitmap,
    .resource_len	= COUNTERS_BITMAP_LEN,
};

void counter_32_bit_read(struct counter_s *counter, u32 *counter_val)
{
    counter_val[0] = atomic_read(counter->cnt_val);
}

void counter_32_bit_write(struct counter_s *counter, u32 *counter_new)
{
    atomic_write(counter->cnt_val, counter_new[0]);
}

void counter_32_bit_add(struct counter_s *counter, u32 *counter_add)
{
    atomic_add(counter->cnt_val, counter_add[0]);
}

void counter_32_bit_sub(struct counter_s *counter, u32 *counter_sub)
{
    atomic_sub(counter->cnt_val, counter_sub[0]);
}

void counter_32_bit_inc(struct counter_s *counter)
{
    atomic_inc(counter->cnt_val);
}

void counter_32_bit_dec(struct counter_s *counter)
{
    atomic_dec(counter->cnt_val);
}

void counter_64_bit_read(struct counter_s *counter, u32 *counter_val)
{
    COUNTER_LOCK(counter);
    counter_val[0] = counter->cnt_val[0];
    counter_val[1] = counter->cnt_val[1];
    COUNTER_UNLOCK(counter);
}

void counter_64_bit_write(struct counter_s *counter, u32 *counter_new)
{
    COUNTER_LOCK(counter);
    counter->cnt_val[0] = counter_new[0];
    counter->cnt_val[1] = counter_new[1];
    COUNTER_UNLOCK(counter);
}

void counter_64_bit_add(struct counter_s *counter, u32 *counter_add)
{
    COUNTER_LOCK(counter);
    add_64_64(counter->cnt_val, counter->cnt_val, counter_add);
    COUNTER_UNLOCK(counter);
}

void counter_64_bit_sub(struct counter_s *counter, u32 *counter_sub)
{
    COUNTER_LOCK(counter);
    sub_64_64(counter->cnt_val, counter->cnt_val, counter_sub);
    COUNTER_UNLOCK(counter);
}

/*
 * "inc" instruction doesn't update the CF flag
 */
void counter_64_bit_inc(struct counter_s *counter)
{
    COUNTER_LOCK(counter);
    add_64_32(counter->cnt_val, counter->cnt_val, 1);
    COUNTER_UNLOCK(counter);
}

/*
 * "dec" instruction doesn't update the CF flag
 */
void counter_64_bit_dec(struct counter_s *counter)
{
    COUNTER_LOCK(counter);
    sub_64_32(counter->cnt_val, counter->cnt_val, 1);
    COUNTER_UNLOCK(counter);
}

struct counter_ops_s counter_32_bit_ops = {
    .cnt_read	= counter_32_bit_read,
    .cnt_write	= counter_32_bit_write,
    .cnt_add	= counter_32_bit_add,
    .cnt_sub	= counter_32_bit_sub,
    .cnt_inc	= counter_32_bit_inc,
    .cnt_dec	= counter_32_bit_dec
};

struct counter_ops_s counter_64_bit_ops = {
    .cnt_read	= counter_64_bit_read,
    .cnt_write	= counter_64_bit_write,
    .cnt_add	= counter_64_bit_add,
    .cnt_sub	= counter_64_bit_sub,
    .cnt_inc	= counter_64_bit_inc,
    .cnt_dec	= counter_64_bit_dec
};

int put_free_counter(int counter_desc)
{
    return put_free_resource(counters_resource_desc, counter_desc);
}

int get_free_counter(void)
{
    return get_free_resource(counters_resource_desc);
}

int register_counter(int counter_type, const char *counter_name) 
{
    int counter_desc;
    
    if ((counter_desc = get_free_counter()) < 0)
	return -1;

    switch(counter_type) {
	case COUNTER_TYPE_32_BIT:
	    printk(MOD_NAME "registering 32 bit ");
	    counters[counter_desc].cnt_ops = &counter_32_bit_ops;
	    break;
	
	case COUNTER_TYPE_64_BIT:
	    printk(MOD_NAME "registering 64 bit ");
	    counters[counter_desc].cnt_ops = &counter_32_bit_ops;
	    break;
	
	default:
	    return -1;
    }    

    printk("%s counter\n", counter_name);
    counters[counter_desc].cnt_name = counter_name;
    
    return counter_desc;
}

int counters_init(void)
{
    printk(MOD_NAME "initializing system counters\n");

    counters_resource_desc = register_resource(&counters_resource);
    counters_sys_init();
    
    return 0;
}
