/*
 * kernel_super/counters/counters_sys.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/09
 * Author: 	Lukasz Dembinski
 * Info:	counters_sys.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>
#include <agnix/counters.h>

int counter_jiffies_desc;

int counters_sys_init(void)
{
    counter_jiffies_desc = register_counter(COUNTER_TYPE_64_BIT, "jiffies");    
    
    return counter_jiffies_desc;
}
