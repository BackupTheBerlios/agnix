#ifndef _AGNIX_MEMORY_MEMORY_H_
#define _AGNIX_MEMORY_MEMORY_H_

#include <asm/types.h>
#include <agnix/list.h>
#include <agnix/memory/alloc.h>
#include <agnix/memory/bin_alloc.h>
#include <agnix/memory/buddy_alloc.h>

struct page_desc_s {
    u32 address_phys;
    u32 address_virt;
    
    struct list_head page_list;
};

void put_free_pages(u32 addr, u8 order);
u32  get_free_pages(u8 order);
int  memalloc_init(void);

#endif
