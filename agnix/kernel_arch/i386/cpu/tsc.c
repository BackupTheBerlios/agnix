/*
 * kernel_arch/i386/kernel/cpu/tsc.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	tsc.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/console.h>
#include <agnix/math64.h>
#include <asm/cpu_ops.h>
#include <agnix/ioport.h>

#define MOD_NAME	"\tTSC: "

extern struct chip_s cpu_chip;

int __init tsc_get_speed(void)
{
    u32 tsc[2];
    u32 tsc_s[2];
    u32 tsc_e[2];

    outb(0xb0, 0x43);
    outb((i8254_RELOAD_COUNT) & 0xFF, 0x42);
    outb((i8254_RELOAD_COUNT >> 8) & 0xFF, 0x42);

    rdtsc(tsc_s[0], tsc_s[1]);

    while(!(inb(0x61) & 0x20));
    
    rdtsc(tsc_e[0], tsc_e[1]);
    
    sub_64_64(tsc, tsc_e, tsc_s);

    return (tsc[0]);    
}

void __init cpu_tsc_init(void)
{
    if (cpu_capable(X86_FEATURE_TSC)) {
	write_cr4(read_cr4() & (~CPU_CR4_TSD));
	
	if (read_cr4() & CPU_CR4_TSD) {
	    printk(MOD_NAME "Tsc cannot be enabled\n");
	}
    }
}
