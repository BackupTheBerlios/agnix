#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/net/net.h>
#include <agnix/console.h>

#define MOD_NAME	"NET: "

int register_net_device_eth(struct netdev_s *netdev)
{
    printk(MOD_NAME "registering %s device\n", netdev->if_name);

    return 0;
}
