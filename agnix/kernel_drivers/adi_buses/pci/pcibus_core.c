/*
 * kernel_drivers/adi_buses/pci/pcibus_core.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/06
 * Author: 	Lukasz Dembinski
 * Info:	pcibus_core.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>

#define MOD_NAME	"PCI: \t"

struct list_head pcibus_list;
struct list_head pcidev_list;
spinlock_t pcibus_list_lock;
spinlock_t pcidev_list_lock;

int pcibus_disable = 0;

int adi_pcibus_init(void)
{
    printk(MOD_NAME "initializing PCI bus\n");

    INIT_LIST_HEAD(&pcibus_list);
    INIT_LIST_HEAD(&pcidev_list);
    spin_lock_init(&pcibus_lock);
    spin_lock_init(&pcidev_lock);

    pcibus_ops_init();
    pcibus_scan_all();
    pcibus_irq_init();

    return 0;
}
