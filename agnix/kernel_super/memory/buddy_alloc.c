/*
 * kernel_super/memory/buddy_alloc.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/09
 * Author: 	Lukasz Dembinski
 * Info:	buddy_alloc.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */
 
#include <agnix/agnix.h>
#include <asm/bitops.h>
#include <agnix/memory.h>
#include <agnix/panic.h>
#include <agnix/console.h>

#define MOD_NAME	"MEM: "
#define NO_PAGE		0

struct buddy_allocator_s buddy_alloc;
extern struct allocator_s main_alloc;

extern void (*put_free_pages_fn)(u32 addr, u8 order);
extern u32  (*get_free_pages_fn)(u8 order);

struct page_desc_s *system_pages;

void buddy_put_free_pages(u32 addr, u8 order)
{
    u8 start_order;
    u32 bitmap_idx;
    struct buddy_area_s *area = &buddy_alloc.buddy_area[order];
    struct page_desc_s *page_desc;
    struct page_desc_s *page_buddy_desc;
    u32 page_idx = virt_to_page(addr);
    u32 page_buddy_idx;
    u32 page_mask;
    u32 flags;

    spin_lock_irqsave(&buddy_alloc.buddy_lock, flags);
    
    page_mask  = (~0UL) << order;
    for (start_order = order; start_order < BUDDY_ALLOC_MAX_AREAS; start_order++) {
	bitmap_idx = page_idx >> (start_order + 1);

	if (!test_and_change_bit(bitmap_idx, area->area_bitmap)) {
	    break;
	}
	
	page_buddy_idx  = page_idx ^ (1 + ~page_mask);
	page_buddy_desc = &system_pages[page_buddy_idx];
	list_del(&page_buddy_desc->page_list);
	area->free_units--;
	
	area++;
	page_mask <<= 1;
	page_idx &= page_mask;
    }

    page_desc = &system_pages[page_idx];
    list_add(&page_desc->page_list, &area->area_list);
    area->free_units++;
    
    spin_unlock_irqrestore(&buddy_alloc.buddy_lock, flags);
}

struct page_desc_s *buddy_get_free_pages_back(u32 order, u32 cur_order)
{
    struct buddy_area_s *area = &buddy_alloc.buddy_area[cur_order];
    struct page_desc_s *page_desc = NULL;
    u32 page_idx;
    u32 page_half_idx;
    u32 back_order;
    u32 area_size;

    page_desc = list_entry(area->area_list.next, struct page_desc_s, page_list);
    page_idx  = virt_to_page(page_desc->address_virt);
    list_del(area->area_list.next);
    area->free_units--;
    area--;
    area_size = 1 << (cur_order - 1);

    for (back_order = order; back_order < cur_order; back_order++) {
	page_half_idx = page_idx + area_size;
	list_add(&page_desc->page_list, &area->area_list);
	area->free_units++;
	set_bit(page_half_idx >> (back_order + 1), area->area_bitmap);
	page_desc = &system_pages[page_half_idx];
	area_size >>= 1;
	area--;
    }   
        
    return page_desc;
}

u32 buddy_get_free_pages(u8 order)
{
    struct buddy_area_s *area = &buddy_alloc.buddy_area[order];
    struct page_desc_s *page_desc = NULL;
    u32 cur_order;
    u32 flags;
        
    spin_lock_irqsave(&buddy_alloc.buddy_lock, flags);

    for (cur_order = order; cur_order < BUDDY_ALLOC_MAX_AREAS; cur_order++) {

	if (!list_empty(&area->area_list)) {
	    if (cur_order == order) {
		page_desc = list_entry(area->area_list.next, struct page_desc_s, page_list);
		list_del(area->area_list.next);
		area->free_units--;
	    } else {
		page_desc = buddy_get_free_pages_back(order, cur_order);
	    }    
	    
	    break;
	}
	area++;
    }
    
    spin_unlock_irqrestore(&buddy_alloc.buddy_lock, flags);

    if (cur_order == BUDDY_ALLOC_MAX_AREAS)
	return 0;
	
    return page_desc->address_virt;
}

void buddy_alloc_fn_init(void)
{
    get_free_pages_fn = buddy_get_free_pages;    
    put_free_pages_fn = buddy_put_free_pages;    
}

int buddy_alloc_areas_init(void)
{
    int i;
    int area_size_pages = 0;
    
    for (i = 0; i < BUDDY_ALLOC_MAX_AREAS; i++) {

	area_size_pages = (((main_alloc.mem_size_pages + (1 << 3) - 1) >> 4) + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
	if ((buddy_alloc.buddy_area[i].area_bitmap = (u32 *)bin_get_free_pages(area_size_pages)) == NULL) {
	    kernel_panic(MOD_NAME "buddy_alloc_init() failed\n");
	}
	
	INIT_LIST_HEAD(&buddy_alloc.buddy_area[i].area_list);
	buddy_alloc.buddy_area[i].free_units = 0;	
    }    
    
    return 0;
}

void buddy_alloc_pages_init(void)
{
    int system_pages_size;
    int i;
    
    system_pages_size = ((main_alloc.mem_size_pages * sizeof(struct page_desc_s)) + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
    
    if ((system_pages = (struct page_desc_s *)bin_get_free_pages(system_pages_size)) == NULL) {
	kernel_panic(MOD_NAME "buddy_alloc_pages_init() failed\n");
    }    
    
    for (i = 0; i < main_alloc.mem_size_pages; i++) {
	system_pages[i].address_phys = (u32)(i << PAGE_SHIFT);
	system_pages[i].address_virt = phys_to_virt(system_pages[i].address_phys);
	INIT_LIST_HEAD(&system_pages[i].page_list);
    }
}

void buddy_alloc_print_units(void)
{
    struct buddy_area_s *area;
    int i;
    u32 flags;
    
    printk(MOD_NAME "buddy_alloc free units\n");

    spin_lock_irqsave(&buddy_alloc.buddy_lock, flags);

    for (i = 0; i < BUDDY_ALLOC_MAX_AREAS; i++) {
	area = &buddy_alloc.buddy_area[i];
	printk(MOD_NAME "\torder %02d -> %d free units\n", i, area->free_units);
    }
    
    spin_unlock_irqrestore(&buddy_alloc.buddy_lock, flags);
}

void __init buddy_alloc_init(void)
{
    spin_lock_init(&buddy_alloc.buddy_lock);

    buddy_alloc_areas_init();
    buddy_alloc_pages_init();
    buddy_alloc_fn_init();
    bin_put_all_free_pages();
    buddy_alloc_print_units();
}
