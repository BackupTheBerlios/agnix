/*
 * /kernel_super/memory/address.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	address.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */


#include <agnix/agnix.h>
#include <agnix/tasks.h>

u32 get_free_addr(struct task_s *task, u32 from, u32 size)
{
    return 0;    
}

void put_free_addr(struct task_s *task, u32 addr, u32 size)
{
}
