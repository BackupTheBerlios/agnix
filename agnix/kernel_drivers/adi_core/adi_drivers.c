/*
 * kernel_drivers/adi_core/adi_drivers.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_drivers.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>

struct list_head adi_pci_drivers_list;
struct list_head adi_usb_drivers_list;
struct list_head adi_isa_drivers_list;

spinlock_t adi_pci_drivers_list_lock;
spinlock_t adi_usb_drivers_list_lock;
spinlock_t adi_isa_drivers_list_lock;

int adi_register_pci_driver(struct adi_driver_s *adi_driver)
{
    u32 flags;

    spin_lock_irqsave(&adi_pci_drivers_list_lock, flags);            
    list_add(&adi_driver->adi_driver_list, &adi_pci_drivers_list);
    spin_unlock_irqrestore(&adi_pci_drivers_list_lock, flags);            

    adi_pcibus_device_init(adi_driver->adi_driver_union.adi_driver_pci, adi_driver->adi_driver_id);

    return 0;
}

int adi_register_driver(struct adi_driver_s *adi_driver)
{
    if (adi_driver->adi_driver_type == ADI_DRIVER_TYPE_PCI)
	adi_register_pci_driver(adi_driver);

    return 0;
}

int adi_drivers_data_init(void)
{
    INIT_LIST_HEAD(&adi_pci_drivers_list);
    INIT_LIST_HEAD(&adi_usb_drivers_list);
    INIT_LIST_HEAD(&adi_isa_drivers_list);

    spin_lock_init(&adi_pci_drivers_list_lock);
    spin_lock_init(&adi_usb_drivers_list_lock);
    spin_lock_init(&adi_isa_drivers_list_lock);

    return 0;
}

int adi_drivers_init(void)
{
    adi_drivers_data_init();
    adi_resources_init();
    adi_buses_init();
#if CONFIG_DRIVERS_NET
    adi_netdrv_init();
#endif
#if CONFIG_DRIVERS_CHAR
    adi_chrdrv_init();
#endif
#if CONFIG_DRIVERS_BLOCK
    adi_blkdrv_init();
#endif

    return 0;
}
