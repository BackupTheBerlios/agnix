#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/net/net.h>
#include <agnix/console.h>
#include <agnix/memory.h>
#include <agnix/strings.h>

int net_devices_last_id = 0;

int net_device_set_name(struct netdev_s *netdev, char *if_name)
{
    int name_len;
    
    name_len = strlen(if_name);

    if (name_len < NETDEV_NAME_MAX_LEN)
	memcpy(netdev->if_name, if_name, name_len);
    else
	memcpy(netdev->if_name, if_name, NETDEV_NAME_MAX_LEN);

    return 0;
}

int net_device_compose_name(struct netdev_s *netdev)
{
    switch(netdev->if_type) {
	case IF_TYPE_ETH:
	    return eth_device_compose_name(netdev);
    }

    return 0;
}

int net_device_set_id(struct netdev_s *netdev)
{
    netdev->if_id = net_devices_last_id;
    
    net_devices_last_id++;
    
    return 0;
}

int net_device_init(struct netdev_s *netdev)
{
    net_device_compose_name(netdev);

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

int register_net_device(struct netdev_s *netdev)
{
    switch (netdev->if_type) {
	case IF_TYPE_ETH:
	    return register_net_device_eth(netdev);
    }

    return 0;
}
