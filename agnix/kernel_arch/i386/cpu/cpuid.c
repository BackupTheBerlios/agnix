/*
 * kernel_arch/i386/kernel/cpu/cpuid.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	cpuid. core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>

void cpuid(u32 level, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
    asm("cpuid"
	:"=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
	:"0"(level));
}

u32 cpuid_reg(u32 level, int reg_nr)
{
    u32 eax, ebx, ecx, edx;

    asm("cpuid"
	:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	:"0"(level));
    
    switch(reg_nr) {
	case 0: return eax;
	case 1: return ebx;
	case 2: return ecx;
	case 3: return edx;
    }

    return 0;
}

