#ifndef _AGNIX_RESOURCES_H_
#define _AGNIX_RESOURCES_H_

#include <agnix/spinlock.h>

struct resource_s {
    const char *	resource_name;
    u32 *		resource_bitmap;
    int 		resource_len;
    spinlock_t 		resource_lock;
};

int get_free_resource(int resource_desc);
int put_free_resource(int resource_desc, int resource_bit);
int register_resource(struct resource_s *resource);
int resources_init(void);

#endif
