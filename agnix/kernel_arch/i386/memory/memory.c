/*
 * kernel_arch/i386/kernel/memory/memory.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	memory.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <asm/bios_parametrs.h>
#include <agnix/memory.h>
#include <asm/memtests.h>
#include <agnix/console.h>

struct main_memory_s  main_memory;
struct memory_block_s main_block;
extern u32 swapper_pg_dir[1024];
extern int page_tables_init(struct main_memory_s *mem);

extern char __init_begin, __init_end;

void __init modify_memory_entry(struct memory_block_s *mem_block, unsigned long long start, unsigned long long size,
		         unsigned long type, int block_nr)
{
    mem_block->entries[block_nr].start = start;
    mem_block->entries[block_nr].size  = size;
    mem_block->entries[block_nr].type  = type;
}

void __init add_memory_entry(struct memory_block_s *mem_block, struct e820_entry_s *entry)
{
    int block_nr = mem_block->mem_nr;

    memcpy(&mem_block->entries[block_nr], entry, sizeof(struct e820_entry_s));
    mem_block->mem_nr++;
}

#define mem_block_size(mem_block, i) \
	mem_block->entries[i].size

#define mem_block_start(mem_block, i) \
	mem_block->entries[i].start

#define mem_block_end(mem_block, i) \
	mem_block->entries[i].start + mem_block->entries[i].size

#define mem_block_type(mem_block, i) \
	mem_block->entries[i].type

unsigned long __init parse_biosmap(struct e820_entry_s *biosentry, char map_nr)
{
    struct e820_entry_s *bios_entry  = biosentry;
    struct e820_entry_s bios_entry_tmp;
    struct memory_block_s *mem_block = &main_block;
    unsigned long max_page  = 0;
    int 	  max_entry;
    unsigned long max_addr;
    unsigned long prev_max_addr = -1;
    int 	  i;
    int 	  map_count = (int)map_nr;

    do {
        max_entry = -1;
	max_addr  = -1;
	for (i = 0; i < map_nr; i++) {
	    if (((prev_max_addr == -1) || (bios_entry[i].start > prev_max_addr)) && (bios_entry[i].start < max_addr)) {
		max_entry = i;
		max_addr = bios_entry[i].start;
	    }	
	}
	if (max_entry == -1) break;
    
	prev_max_addr = max_addr;
	
	memcpy(&bios_entry_tmp, &bios_entry[max_entry], sizeof(bios_entry_tmp));

	for (i = 0; i < mem_block->mem_nr; i++) {
	    if (bios_entry_tmp.start < mem_block_end(mem_block, i)) {
		if (bios_entry_tmp.type > mem_block_type(mem_block, i)) {
		    mem_block_size(mem_block, i) = mem_block_start(mem_block, i) - 
					       bios_entry_tmp.start;
		} else {
		    bios_entry_tmp.start = mem_block_end(mem_block, i);
		}
	    }
	}

	add_memory_entry(mem_block, &bios_entry_tmp);
	if (max_page < bios_entry_tmp.start + bios_entry_tmp.size)
	    max_page = bios_entry_tmp.start + bios_entry_tmp.size;
    
	map_count--;
    } while(map_count);

    return max_page;
}

void print_memory_map(struct main_memory_s *memory)
{
    int i;
    char *e820_name;
    
    printk("Memory size = %dKB\n", (unsigned long)(memory->mem_size));
    printk("Memory map (BIOS):\n");
    
    for (i = 0; i < main_block.mem_nr; i++) {
    
	if (main_block.entries[i].size == 0)
	    continue;
    
	switch (main_block.entries[i].type) {
	
	    case E820_RAM:
		e820_name = "(RAM)     ";
		break;
	    case E820_RESERVED:
		e820_name = "(RESERVED)";
		break;
	    case E820_ACPI:
		e820_name = "(ACPI)    ";
		break;
	    case E820_NVS:
		e820_name = "(NVS)     ";
		break;
	    default:
		e820_name = "(UNKNOWN) ";
	}
	
	printk("\t%s %08x-%08x\n", e820_name, (unsigned long)main_block.entries[i].start, 
	       (unsigned long)(main_block.entries[i].start + main_block.entries[i].size));
    }

    printk("\n");    
}

unsigned long __init parse_memory_size(void)
{
    if (MEM_SIZE_1 < MEM_SIZE_2)
	return MEM_SIZE_2;
    else
	return MEM_SIZE_1;
}

int __init bootmem_init_bh(void)
{
    memset(&main_memory, 0, sizeof(main_memory));
    memset(&main_block, 0, sizeof(main_block));
    INIT_LIST_HEAD(&main_memory.list);
    INIT_LIST_HEAD(&main_block.list);
    list_add(&main_block.list, &main_memory.list);
    
    main_memory.mem_size = parse_memory_size();
    main_memory.pdbr = (u32)swapper_pg_dir;

    return 0;
}

int __init bootmem_init_dh(void) 
{
    page_tables_init(&main_memory);

    parse_biosmap(E820_MAP, E820_MAP_NR);
    print_memory_map(&main_memory);

#if CONFIG_MEMTESTS
    if (do_memtests() < 0) {
	return -1;
    }
#endif

    return 0;
}

int bootmem_free_init(void)
{
    u32 init_start = (u32)&__init_begin;
    u32 init_end = (u32)&__init_end;

    printk("Freeing __init segment (%d kb)\n", (init_end - init_start) >> 10);
	
    return 0;
}
