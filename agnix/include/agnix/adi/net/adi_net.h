#ifndef _AGNIX_DRIVERS_CHIPSDRV_H_
#define _AGNIX_DRIVERS_CHIPSDRV_H_

#include <agnix/list.h>
#include <agnix/net/net.h>
#include <agnix/adi/net/adi_net_rtl_8029.h>

#define NETDEV_NAME_MAX_LEN	32
#define NETDEV_MAC_LEN		6

struct adi_netdev_s;

struct adi_netdev_ops_s {
    int (*driver_open)(struct adi_netdev_s *netdev);
    int (*driver_close)(struct adi_netdev_s *netdev);
    int (*transmit_frame)(struct adi_netdev_s *netdev, struct frb_s *frb, int frame_len);
    int (*set_recv_mode)(struct adi_netdev_s *netdev, int recv_mode);
    int (*set_mac_addr)(struct adi_netdev_s *netdev, u8 *mac_addr);
    int (*set_broadcast_addr)(struct adi_netdev_s *netdev, u8 *broadcast_addr);
    int (*set_mtu)(struct adi_netdev_s *netdev, int mtu);
};

struct adi_netdev_primitives_s {
    int (*header_build)(struct frb_s *frb, u8 *dest_addr, u8 *src_addr, int proto);
    int (*header_parse)(struct frb_s *frb);
};

struct adi_netdev_s {
    int  if_type;
    int  if_id;
    char if_name[NETDEV_NAME_MAX_LEN];
    u8   if_addr[NETDEV_MAC_LEN];
    u8   if_addr_broadcast[NETDEV_MAC_LEN];
    int  if_addr_len;
    int  if_mtu;

    struct adi_netdev_ops_s *if_ops;
    struct adi_netdev_primitives_s *if_primit;
    
    void *if_layer_2_data;
    void *if_layer_3_data;
    void *if_layer_4_data;
};

int adi_register_netdev(struct adi_netdev_s *netdev);
int adi_netdev_init(struct adi_netdev_s *netdev);
int adi_netdrv_init(void);

#endif
