#ifndef _AGNIX_DRIVERS_CHIPSDRV_H_
#define _AGNIX_DRIVERS_CHIPSDRV_H_

#include <agnix/list.h>

#include <agnix/adi/net/adi_net_rtl_8029.h>


/*#define NETDEV_MAC_LEN		6
#define NETDEV_NAME_LEN		32

#define IF_TYPE_ETH		0x0000
#define IF_TYPE_TR		0x0002

struct netdev_s {
    char if_name[NETDEV_NAME_LEN];
    u8 if_addr[NETDEV_MAC_LEN];
    u8 if_addr_broadcast[NETDEV_MAC_LEN];
    int if_addr_len;
    int if_type;
};

int adi_register_netdev(struct netdev_s *netdev);
*/

int adi_netdrv_init(void);

#endif
