#ifndef _AGNIX_NET_FRB_H_
#define _AGNIX_NET_FRB_H_

#define FRB_DEST_TYPE_LOCAL		0
#define FRB_DEST_TYPE_BROADCAST		1
#define FRB_DEST_TYPE_MULTICAST		2
#define FRB_DEST_TYPE_OTHER		3
    
struct adi_netdev_s;

struct frb_s {
    u8 *frb_data;
    int frb_len;

    struct adi_netdev_s *frb_dev;
    int frb_proto;
    int frb_dest_type;
    int frb_offset;
};

#endif
