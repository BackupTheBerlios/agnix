/*
 * kernel_super/memory/bin_alloc.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	bin_alloc.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */
 
#include <agnix/init.h>
#include <asm/types.h>
#include <asm/bitops.h>
#include <asm/memory.h>
#include <asm/paging.h>

#include <agnix/memory.h>
#include <agnix/bugs.h>
#include <agnix/console.h>

#define MOD_NAME	"MEM: "
#define NO_PAGE		0

struct bin_allocator_s bin_alloc;
extern struct allocator_s main_alloc;

extern void (*put_free_pages_fn)(u32 addr, u8 order);
extern u32  (*get_free_pages_fn)(u8 order);

void bin_reserve_pages(struct bin_allocator_s *allocator, u32 start_page, u32 size)
{
    u32 idx_start = start_page - main_alloc.mem_start;
    u32 idx_end = idx_start + size;
    u32 i;
    
    for (i = idx_start; i < idx_end; i++) {
	if (test_and_set_bit((int)i, (void *)allocator->mem_table)) {
	    BUG(MOD_NAME, BUG_RESERVE_PAGES);
	    return;
	}
    }
}

void bin_free_pages(struct bin_allocator_s *allocator, u32 start_page, u32 size)
{
    u32 idx_start = start_page - main_alloc.mem_start;
    u32 idx_end = idx_start + size;
    u32 i;
    
    for (i = idx_start; i < idx_end; i++) {
	if (!test_and_clear_bit((int)i, (void *)allocator->mem_table)) {
	    BUG(MOD_NAME, BUG_FREE_PAGES);
	    return;
	}
    }
}

u32 _bin_get_free_pages(struct bin_allocator_s *allocator, u32 size)
{
    u32 i = 0;
    u32 j = 0;

    for (i = 0; i < main_alloc.mem_size_pages; i++) {
	if (test_bit((int)i, (void *)allocator->mem_table))
	    continue;
	
	for (j = 0; j < size; j++) {
	    if (test_bit((int)(i + j), (void *)allocator->mem_table))
		goto bad_area;
	}
	
	return (main_alloc.mem_start + i);
	
bad_area:
	i += j;
    }
    
    return NO_PAGE;
}

void bin_put_free_pages(u32 addr, u32 pages)
{
    bin_free_pages(&bin_alloc, (addr - PAGE_OFFSET) >> PAGE_SHIFT, pages);    
}

void bin_put_free_pages_order(u32 addr, u8 order)
{
    return bin_put_free_pages(addr, (u32)(1 << order));    
}

u32 bin_get_free_pages(u32 pages)
{
    u32 page;

    page = _bin_get_free_pages(&bin_alloc, pages);
    
    if (page != NO_PAGE)
	bin_reserve_pages(&bin_alloc, page, pages);
    else
	return NO_PAGE;

    return ((page << PAGE_SHIFT) + PAGE_OFFSET);
}

u32 bin_get_free_pages_order(u8 order)
{
    return bin_get_free_pages((u32)(1 << order));
}

int bin_put_all_free_pages(void)
{
    int i;
    int free_count = 0;
    
    for (i = 0; i < main_alloc.mem_size_pages; i++) {
	if (test_bit((int)i, (void *)(bin_alloc.mem_table)))
	    continue;
	
	put_free_pages(phys_to_virt((u32)(i << PAGE_SHIFT)), 0);
	free_count++;
    }

    for (i = virt_to_page((u32)(bin_alloc.mem_table)); i < main_alloc.mem_table_size_pages; i++) {
	put_free_pages(phys_to_virt((u32)(i << PAGE_SHIFT)), 0);
	free_count++;
    }

    printk(MOD_NAME "there are %d free pages (%dKB)\n", free_count, free_count << 2);
    printk(MOD_NAME "mem_size=%d pages, mem_table=%d pages\n", main_alloc.mem_size_pages, main_alloc.mem_table_size_pages);
        
    return free_count;
}

void bin_alloc_fn_init(void)
{
    get_free_pages_fn = bin_get_free_pages_order;    
    put_free_pages_fn = bin_put_free_pages_order;    
}

void __init bin_alloc_init(u32 *mem_table, u32 mem_start, u32 mem_end)
{
    memset(&bin_alloc, 0, sizeof(bin_alloc));
    
    bin_alloc.mem_table  = mem_table;
    main_alloc.mem_start = mem_start;
    main_alloc.mem_end   = mem_end;
    main_alloc.mem_size  = mem_end - mem_start;
    main_alloc.mem_size_pages       = main_alloc.mem_size >> 2;
    main_alloc.mem_table_size_pages = (((main_alloc.mem_size_pages + ((1 << 3) - 1)) >> 3) + (PAGE_SIZE - 1)) >> PAGE_SHIFT;

    memset(mem_table, 0, main_alloc.mem_size);
        
    bin_alloc_fn_init();
    bin_reserve_pages(&bin_alloc, 0, 2);
    bin_reserve_pages(&bin_alloc, virt_to_phys((u32)(mem_table)) >> PAGE_SHIFT, 
		       main_alloc.mem_table_size_pages);
}

