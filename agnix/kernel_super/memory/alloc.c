/*
 * kernel_super/memory/alloc.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	alloc.c core file
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

extern char _end;
extern struct main_memory_s main_memory;

//extern int bootmem_init_bh(void);
//extern int bootmem_init_dh(void);

void (*put_free_pages_fn)(u32 addr, u8 order);
u32  (*get_free_pages_fn)(u8 order);

struct allocator_s main_alloc;

void put_free_pages(u32 addr, u8 order)
{
    return (*put_free_pages_fn)(addr, order);
}

u32 get_free_pages(u8 order)
{
    return (*get_free_pages_fn)(order);
}

int __init memalloc_init(void)
{
    bootmem_init_bh();
    bin_alloc_init((u32 *)&_end, 0, main_memory.mem_size);
    bootmem_init_dh();
    buddy_alloc_init();
    
    return 0;
}
