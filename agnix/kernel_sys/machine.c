/*
 * kernel_sys/machine.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	machine.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>
#include <asm/machine.h>

#define MOD_NAME	"MACHINE: "

int do_sys_reboot(void)
{
    printk(MOD_NAME "System is rebooting ...");
    
    do_machine_reboot();    

    return 0;
}
