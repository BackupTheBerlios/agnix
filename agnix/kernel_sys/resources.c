#include <agnix/agnix.h>
#include <agnix/resources.h>
#include <agnix/spinlock.h>
#include <agnix/memory.h>
#include <agnix/console.h>
#include <asm/cpu_ops.h>

#define MOD_NAME		"RES: "
#define GLOBAL_RESOURCE_LEN	16
#define GLOBAL_RESOURCE_DESC	0

struct resource_s *resources[GLOBAL_RESOURCE_LEN];

u32 global_resource_bitmap[(GLOBAL_RESOURCE_LEN << 5) + 1];

struct resource_s global_resource = {
    .resource_name 	= "global",
    .resource_bitmap 	= global_resource_bitmap,
    .resource_len	= GLOBAL_RESOURCE_LEN,
};

int get_free_resource(int resource_desc)
{
    struct resource_s * resource = resources[resource_desc];
    u32 *bitmap;
    int bitmap_len;
    int zero_bit;
    u32 flags;

    bitmap = resource->resource_bitmap;
    bitmap_len = resource->resource_len << 5;

    spin_lock_irqsave(&resource->resource_lock, flags);
    if ((zero_bit = find_first_zero_bit(bitmap, bitmap_len)) < bitmap_len)
        set_bit(zero_bit, bitmap);
    else
	zero_bit = -1;
	
    spin_unlock_irqrestore(&resource->resource_lock, flags);
    
    return zero_bit;
}

int put_free_resource(int resource_desc, int resource_bit)
{
    struct resource_s * resource = resources[resource_desc];
    u32 *bitmap;
    u32  flags;
    int  ret = 0;

    if (resource == NULL) {
	printk(MOD_NAME "resource is NULL\n");
	return -1;
    }
    
    bitmap = resource->resource_bitmap;

    spin_lock_irqsave(&resource->resource_lock, flags);
    if (!test_and_clear_bit(resource_bit, bitmap)) {
	printk(MOD_NAME "there is zero bit on %d position in a queue_bitmap", resource_bit);
	ret = -1;
    }
    spin_unlock_irqrestore(&resource->resource_lock, flags);

    return ret;
}

int register_resource(struct resource_s *resource)
{
    int resource_desc;

    printk(MOD_NAME "Registering %s resource\n", resource->resource_name);
    
    resource_desc = get_free_resource(GLOBAL_RESOURCE_DESC);
    resources[resource_desc] = resource;    
    memset(resource->resource_bitmap, 0, resource->resource_len);

    return resource_desc;
}

void unregister_resource(int resource_desc)
{
}

int resources_init(void)
{
    int i;

    printk(MOD_NAME "Initializing %s resource\n", global_resource.resource_name);
    
    spin_lock_init(&global_resource.resource_lock);    
    
    /* sanity loop */
    for (i = 0; i < GLOBAL_RESOURCE_LEN; i++) {
	resources[i] = NULL;
    }
    resources[GLOBAL_RESOURCE_DESC] = &global_resource;
    set_bit(GLOBAL_RESOURCE_DESC, global_resource_bitmap);
    
    return 0;
}
