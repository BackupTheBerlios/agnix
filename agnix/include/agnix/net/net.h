#ifndef _AGNIX_NET_NET_H_
#define _AGNIX_NET_NET_H_

#include <agnix/net/frb.h>
#include <agnix/net/layer_2/eth.h>

#define NETDEV_MAC_LEN		6
#define NETDEV_NAME_MAX_LEN		32

#define IF_TYPE_ETH		0x0000
#define IF_TYPE_TR		0x0001
#define IF_TYPE_FDDI		0x0002

struct netdev_s;

struct netdev_ops_s {
    int (*driver_open)(struct netdev_s *netdev);
    int (*driver_close)(struct netdev_s *netdev);
    int (*transmit_frame)(struct netdev_s *netdev, struct frb_s *frb, int frame_len);
    int (*set_recv_mode)(struct netdev_s *netdev, int recv_mode);
    int (*set_mac_addr)(struct netdev_s *netdev, u8 *mac_addr);
    int (*set_broadcast_addr)(struct netdev_s *netdev, u8 *broadcast_addr);
    int (*set_mtu)(struct netdev_s *netdev, int mtu);
};

struct netdev_primitives_s {
    int (*header_build)(struct frb_s *frb, u8 *dest_addr, u8 *src_addr, int proto);
    int (*header_parse)(struct frb_s *frb);
};

struct netdev_s {
    int  if_type;
    int  if_id;
    char if_name[NETDEV_NAME_MAX_LEN];
    u8   if_addr[NETDEV_MAC_LEN];
    u8   if_addr_broadcast[NETDEV_MAC_LEN];
    int  if_addr_len;
    int  if_mtu;

    struct netdev_ops_s *if_ops;
    struct netdev_primitives_s *if_primit;
    
    void *if_layer_2_data;
    void *if_layer_3_data;
    void *if_layer_4_data;
};

int register_net_device(struct netdev_s *netdev);
int net_device_init(struct netdev_s *netdev);

#endif
