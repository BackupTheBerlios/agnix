#ifndef _AGNIX_MEMORY_BIN_ALLOC_H_
#define _AGNIX_MEMORY_BIN_ALLOC_H_

#include <asm/types.h>
#include <agnix/list.h>

struct bin_allocator_s {
    u32 *mem_table;		
//    u32 mem_start;		/* start page */
//    u32 mem_end;		/* end page */
//    u32 mem_size;
//    u32 mem_size_pages;
//    u32 mem_table_size_pages;
};

void bin_alloc_init(u32 *mem_table, u32 mem_start, u32 mem_end);
void bin_reserve_pages(struct bin_allocator_s *allocator, u32 start_page, u32 size);
int  bin_put_all_free_pages(void);
void bin_put_free_pages_order(u32 addr, u8 order);
void bin_put_free_pages(u32 addr, u32 order);
u32  bin_get_free_pages_order(u8 order);
u32  bin_get_free_pages(u32 order);

#endif
