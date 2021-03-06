#ifndef _AGNIX_NET_LAYER_2_ETH_H_
#define _AGNIX_NET_LAYER_2_ETH_H_

#include <agnix/net/net.h>

#define NET_LAYER_2_ETH_MAC_LEN		6
#define NET_LAYER_2_ETH_MTU		1500

struct adi_netdev_s;

int eth_device_compose_name(struct adi_netdev_s *netdev);
int eth_device_init(struct adi_netdev_s *netdev);
int register_net_device_eth(struct adi_netdev_s *netdev);

#endif
