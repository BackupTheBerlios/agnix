/*
 * kernel_drivers/adi_core/adi_buses.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_buses.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/console.h>

#define MOD_NAME		"BUSES: "

const char *buses_class_names[] = {
    "ISA", "MCA", "PCI", "USB"
};

int adi_register_bus(void)
{
    return 0;
}

int __init adi_buses_init(void)
{
    printk(MOD_NAME "Initializing buses\n");

    adi_pcibus_init();
//  adi_usbbus_init();
//  adi_isabus_init();
    
    return 0;
}
