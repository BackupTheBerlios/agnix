#ifndef _AGNIX_COUNTERS_COUNTERS_H_
#define _AGNIX_COUNTERS_COUNTERS_H_

#include <agnix/agnix.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>

#define MAX_COUNTERS		32
#define MAX_COUNTER_NAME_LEN	32
#define MAX_COUNTER_LEN		2
#define COUNTERS_BITMAP_LEN	(((MAX_COUNTERS - 1) >> 5) + 1)

#define COUNTER_TYPE_32_BIT	0
#define COUNTER_TYPE_64_BIT	1

#define counter_read(cnt_desc, buf)	do { counters[cnt_desc].cnt_ops->cnt_read(&counters[cnt_desc], buf); } while(0)
#define counter_write(cnt_desc, buf)	do { counters[cnt_desc].cnt_ops->cnt_write(&counters[cnt_desc], buf); } while(0)
#define counter_inc(cnt_desc) 		do { counters[cnt_desc].cnt_ops->cnt_inc(&counters[cnt_desc]); } while(0)
#define counter_dec(cnt_desc) 		do { counters[cnt_desc].cnt_ops->cnt_dec(&counters[cnt_desc]); } while(0)
#define counter_add(cnt_desc, val) 	do { counters[cnt_desc].cnt_ops->cnt_add(&counters[cnt_desc], val); } while(0)
#define counter_sub(cnt_desc, val) 	do { counters[cnt_desc].cnt_ops->cnt_sub(&counters[cnt_desc], val); } while(0)

struct counter_s;

struct counter_ops_s {
    void (*cnt_read)(struct counter_s *counter, u32 *counter_val);
    void (*cnt_write)(struct counter_s *counter, u32 *counter_new);
    void (*cnt_add)(struct counter_s *counter, u32 *counter_add);
    void (*cnt_sub)(struct counter_s *counter, u32 *counter_sub);
    void (*cnt_inc)(struct counter_s *counter);
    void (*cnt_dec)(struct counter_s *counter);
};

struct counter_s {
    const char *	    cnt_name;
    int 	     	    cnt_type;
    spinlock_t 	     	    cnt_lock;
    struct list_head 	    cnt_list;
    u32 		    cnt_val[MAX_COUNTER_LEN];
    
    struct counter_ops_s    *cnt_ops;
};

extern struct counter_s counters[MAX_COUNTERS];

int register_counter	 (int counter_type, const char *counter_name);
int unregister_ceounter	 (int counter_desc);
int counters_init	 (void);
int counters_sys_init	 (void);

#endif

