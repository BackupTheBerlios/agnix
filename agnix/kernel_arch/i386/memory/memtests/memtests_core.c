/*
 * kernel_arch/i386/kernel/memory/memtests.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/07
 * Author: 	Lukasz Dembinski
 * Info:	memtests.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/memory.h>
#include <agnix/linkage.h>
#include <asm/memtests.h>
#include <agnix/console.h>

#define MOD_NAME	"MEMTEST: "

extern struct main_memory_s main_memory;

u32 memaddr_next_1(u32 addr)
{
    return addr + 16;
}

u32 memdata_next_1(u32 data)
{
    return 0xA5A5A5A5;
}


int memcheck_type_1(u32 addr_check, u32 data_check)
{
    __asm__ __volatile (
    "movl (%%edi), %%eax\n\t"
    "subl %%ebx, %%eax"
    :
    :"D"(addr_check), "b"(data_check)
    );

    /* return value is in eax. We've got a warrning */
//    return 0;
}

struct memtest_addr_pattern_s memaddr_patterns[MEMTEST_MAX_ADDR_PATTERNS] = {
    {
    .memtest_addr_pattern_type		= MEMTEST_ADDR_PATTERN_ALL,
    .next_addr				= memaddr_next_1
    },
    { 0 }
};

struct memtest_data_pattern_s memdata_patterns[MEMTEST_MAX_DATA_PATTERNS] = {
    {
    .memtest_data_pattern_type		= MEMTEST_DATA_PATTERN_BINARY,
    .next_data				= memdata_next_1
    },
    { 0 }
};

struct memtest_check_type_s memcheck_types[MEMTEST_MAX_CHECK_TYPES] = {
    {
    .memtest_check_type			= MEMTEST_CHECK_RW,
    .check_mem				= memcheck_type_1
    },
    { 0 }
};

int memtests_init(void)
{
    printk(MOD_NAME "initializing memory tests\n");
    
    return 0;
}

u32 do_memtest_next_addr(u32 addr_cur, int memaddr_pattern_type)
{
    u32 addr_next = addr_cur;
    int i;
        
    for (i = 0; i < MEMTEST_MAX_ADDR_PATTERNS; i++) {
	if (!memaddr_patterns[i].memtest_addr_pattern_type)
	    return addr_next;
	
	if (!(memaddr_patterns[i].memtest_addr_pattern_type & memaddr_pattern_type))
	    continue;
	
	addr_next = memaddr_patterns[i].next_addr(addr_next);
    }
    
    return addr_next;
}

u32 do_memtest_next_data(u32 data_cur, int memdata_pattern_type)
{
    u32 data_next = data_cur;
    int i;
        
    for (i = 0; i < MEMTEST_MAX_DATA_PATTERNS; i++) {
	if (!memdata_patterns[i].memtest_data_pattern_type)
	    return data_next;
	
	if (!(memdata_patterns[i].memtest_data_pattern_type & memdata_pattern_type))
	    continue;
	
	data_next = memdata_patterns[i].next_data(data_next);
    }
    
    return data_next;
}

int do_memtest_checks(u32 addr_check, u32 data_check, int memtest_check_type)
{
    int i;
    int check_ret;
        
    for (i = 0; i < MEMTEST_MAX_CHECK_TYPES; i++) {
	if (!memcheck_types[i].memtest_check_type)
	    return 0;
	
	if (!(memcheck_types[i].memtest_check_type & memtest_check_type))
	    continue;
	
	if ((check_ret = memcheck_types[i].check_mem(addr_check, data_check)) < 0)
	    return check_ret;
    }
    
    return 0;
}

int do_memtest_check(u32 addr_start, u32 addr_end, u32 data_start,
		     int memaddr_pattern_type, int memdata_pattern_type, int memtest_check_type)
{
    u32 addr_check_cur = addr_start;
    u32 data_check_cur = data_start;

    do {

	if (do_memtest_checks(addr_check_cur, data_check_cur, memtest_check_type) < 0) {
	    printk(MOD_NAME "check failed (%08x, %08x)\n", addr_check_cur, data_check_cur);
	    return -1;
	}

	__asm__ __volatile (
	"movl %%eax, (%%edi)"
	:
	:"D"(addr_check_cur), "a"(0)
	);

	addr_check_cur = do_memtest_next_addr(addr_check_cur, memaddr_pattern_type);
	data_check_cur = do_memtest_next_data(data_check_cur, memdata_pattern_type);

    } while(addr_check_cur < addr_end);
    
    return 0;
}

int do_memtest(u32 addr_start, u32 addr_end, int memaddr_pattern_type, int memdata_pattern_type, int memtest_check_type, int check_rate)
{
    u32 addr_cur = addr_start;
    u32 data_cur = 0xA5A5A5A5;
    u32 addr_last_check = addr_cur;
    u32 data_last_check = data_cur;
    int check_rate_cur = 0;
    int check_ret = 0;
    int bar_progress = 0;
    int bar_next_progress;
    u32 bar_max_progress = (addr_end - addr_start);
    int i;

    do {
	
	check_rate_cur++;
	
	if (check_rate == check_rate_cur) {
	    check_rate_cur = 0;
	    check_ret = do_memtest_check(addr_last_check, addr_cur, data_last_check,
			     memaddr_pattern_type, memdata_pattern_type, memtest_check_type);

	    if (check_ret < 0) {
		printk(" failed\n");
		return check_ret;
	    }

	    addr_last_check = addr_cur;
	    data_last_check = data_cur;
	    
	    bar_next_progress = ((addr_cur - addr_start) * 10) / bar_max_progress;
	    if (bar_next_progress != bar_progress) {
		for (i = bar_progress; i < bar_next_progress; i++) {
		    printk(".");
		}
		bar_progress = bar_next_progress;
	    }
     	}

	__asm__ __volatile (
	"movl %%eax, (%%edi)"
	:
	:"D"(addr_cur), "a"(data_cur)
	);

	addr_cur = do_memtest_next_addr(addr_cur, memaddr_pattern_type);
	data_cur = do_memtest_next_data(data_cur, memdata_pattern_type);

    } while (addr_cur < addr_end);

    printk(" ok\n");

    return 0;
}

int do_memtests(void)
{	
    printk(MOD_NAME "upper memory tests....\n");
    
    printk(MOD_NAME "RW test ");

    do_memtest(0xc0200000, 0xc0000000 + (main_memory.mem_size << 10), MEMTEST_ADDR_PATTERN_ALL, 
	       MEMTEST_DATA_PATTERN_BINARY, MEMTEST_CHECK_RW, 32);
    
    return 0;
}

