/*
 * kernel_arch/i386/kernel/fastrand.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	fastrand.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>

#define RAND_CONST	33614

unsigned int random_val = 1234;

unsigned int _fastrand(u32 value)
{
    unsigned int result;

    __asm__ ("mull %%ebx\n\t"
	     "addl %%edx, %%eax"
    :"=a"(result)
    :"a"(value), "b"(RAND_CONST)
    );
    
    return result;
}

unsigned int fastrand(u32 range)
{
    random_val = _fastrand(random_val);
    
    return random_val % range;
}

