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
#include <agnix/memory.h>
#include <agnix/strings.h>
#include <agnix/console.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>

#define MOD_NAME		"NET: "

struct list_head 	adi_netdev_list;
spinlock_t 		adi_netdev_list_lock;

int net_devices_last_id = 0;

int adi_netdev_set_name(struct adi_netdev_s *netdev, char *if_name)
{
    int name_len;
    
    name_len = strlen(if_name);

    if (name_len < NETDEV_NAME_MAX_LEN)
	memcpy(netdev->if_name, if_name, name_len);
    else
	memcpy(netdev->if_name, if_name, NETDEV_NAME_MAX_LEN);

    return 0;
}

int adi_netdev_compose_name(struct adi_netdev_s *netdev)
{
    switch(netdev->if_type) {
	case IF_TYPE_ETH:
	    return eth_device_compose_name(netdev);
    }

    return 0;
}

int adi_netdev_set_id(struct adi_netdev_s *netdev)
{
    netdev->if_id = net_devices_last_id;
    
    net_devices_last_id++;
    
    return 0;
}

int adi_netdev_init(struct adi_netdev_s *netdev)
{
    adi_netdev_compose_name(netdev);

    switch(netdev->if_type) {
	case IF_TYPE_ETH:
	    return eth_device_init(netdev);

	case IF_TYPE_FDDI:
	    break;

	case IF_TYPE_TR:
	    break;
    }

    return 0;
}

int adi_register_netdev(struct adi_netdev_s *netdev)
{
    switch (netdev->if_type) {
	case IF_TYPE_ETH:
	    return register_net_device_eth(netdev);
    }

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


