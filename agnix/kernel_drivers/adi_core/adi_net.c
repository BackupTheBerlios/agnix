/*
 * kernel_drivers/adi/adi_net.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_net.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/init.h>
#include <agnix/adi/adi.h>
#include <agnix/net/net.h>
#include <agnix/irq.h>
#include <agnix/console.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>

#define MOD_NAME		"NET: "

struct list_head adi_netdev_list;
spinlock_t adi_netdev_list_lock;

int adi_register_netdev(struct netdev_s *netdev)
{
    return 0;
}

int adi_netdev_data_init(void)
{	
    INIT_LIST_HEAD(&adi_netdev_list);
    spin_lock_init(&adi_netdev_list_lock);

    return 0;
}

int __init adi_netdrv_init(void)
{
    printk(MOD_NAME "Initializing network drivers\n");

    adi_netdev_data_init();

#if CONFIG_DRIVERS_NET_RTL_8029
    adi_netdrv_rtl_8029_init();
#endif
#if CONFIG_DRIVERS_NET_3c59x
    adi_netdrv_3c59x_init();
#endif
    
    return 0;
}
