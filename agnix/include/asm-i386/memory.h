#ifndef _AGNIX_ASM_MEMORY_H_
#define _AGNIX_ASM_MEMORY_H_

#define E820_MAX_ENTRIES	32
#define E820_RAM		1
#define E820_RESERVED		2
#define E820_ACPI		3
#define E820_NVS		4

#include <agnix/list.h>
#include <asm/paging.h>
#include <asm/types.h>

struct e820_entry_s {
    unsigned long long start;
    unsigned long long size;
    unsigned long type;
};

struct memory_block_s {
    struct list_head list;    
    int mem_nr;
    struct e820_entry_s entries[E820_MAX_ENTRIES];
};

struct main_memory_s {
    struct list_head list;
    u32 mem_size;
    u32 lowmem_size;
    u32 highmem_size;
    u32 pdbr;
};

int bootmem_init_bh(void);
int bootmem_init_dh(void);

#endif
