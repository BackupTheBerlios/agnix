#ifndef _AGNIX_MEMORY_BUDDY_ALLOC_H_
#define _AGNIX_MEMORY_BUDDY_ALLOC_H_

#include <asm/types.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>

#define BUDDY_ALLOC_MAX_AREAS		16

struct buddy_area_s {
    u32 *area_bitmap;
    struct list_head area_list;
    int free_units;
};

struct buddy_allocator_s {
    struct buddy_area_s buddy_area[BUDDY_ALLOC_MAX_AREAS];

    spinlock_t buddy_lock;
};

void buddy_alloc_init(void);
//void buddy_reserve_pages(struct bin_allocator_s *allocator, u32 start_page, u32 size);

#endif
