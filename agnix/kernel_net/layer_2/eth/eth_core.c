#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/init.h>
#include <agnix/net/net.h>
#include <agnix/memory.h>
#include <agnix/console.h>
#include <agnix/sprintf.h>

#define MOD_NAME	"NET: "

/* device locks!!! */

int eth_device_nr = 0;

int eth_header_build(struct frb_s *frb, u8 *dest_addr, u8 *src_addr, int proto)
{
    return 0;
}

int eth_header_parse(struct frb_s *frb)
{
    if (frb->frb_data[0] & 0x01) {
	if (!memcmp(frb->frb_data, frb->frb_dev->if_addr_broadcast, NET_LAYER_2_ETH_MAC_LEN))
	    frb->frb_dest_type = FRB_DEST_TYPE_BROADCAST;
    } else
    if (!memcmp(frb->frb_data, frb->frb_dev->if_addr, NET_LAYER_2_ETH_MAC_LEN))
	frb->frb_dest_type = FRB_DEST_TYPE_LOCAL;
    else
	frb->frb_dest_type = FRB_DEST_TYPE_OTHER;

    return 0;
}

int eth_set_mtu(struct adi_netdev_s *netdev, int mtu)
{
    return 0;
}

int eth_set_mac_addr(struct adi_netdev_s *netdev, u8 *mac_addr)
{
    return 0;
}

int eth_set_broadcast_addr(struct adi_netdev_s *netdev, u8 *broadcast_addr)
{
    memset(netdev->if_addr_broadcast, 0xff, NET_LAYER_2_ETH_MAC_LEN);

    return 0;
}

struct adi_netdev_primitives_s ethdev_primit = {
    .header_build	= eth_header_build,
    .header_parse	= eth_header_parse
};

struct adi_netdev_ops_s ethdev_ops = {
    .set_mac_addr	= eth_set_mac_addr,
    .set_broadcast_addr	= eth_set_broadcast_addr,
    .set_mtu		= eth_set_mtu
};

int eth_device_compose_name(struct adi_netdev_s *netdev)
{
    sprintf(netdev->if_name, "eth%d", eth_device_nr);    
    eth_device_nr++;

    return 0;
}

int eth_device_init(struct adi_netdev_s *netdev)
{
    netdev->if_primit 	= &ethdev_primit;
    netdev->if_ops	= &ethdev_ops;
    netdev->if_addr_len	= NET_LAYER_2_ETH_MAC_LEN;
    netdev->if_mtu	= NET_LAYER_2_ETH_MTU;

    eth_set_broadcast_addr(netdev, (u8 *)0);
    
    return 0;
}
