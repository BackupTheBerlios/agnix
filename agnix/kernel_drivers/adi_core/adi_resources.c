/*
 * kernel_drivers/adi_core/adi_resources.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_resources.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>

#define MOD_NAME		"ADI: "
#define MAX_GLOBAL_RESOURCES	16

LIST_HEAD(adi_resources_mem_list);
spinlock_t adi_resources_mem_lock;

LIST_HEAD(adi_resources_io_list);
spinlock_t adi_resources_io_lock;

struct adi_resource_s global_io_resources[MAX_GLOBAL_RESOURCES] = {
    { "dma", 	  0x00, 0x1f, ADI_RES_FLAG_IO },
    { "pic", 	  0x20, 0x3f, ADI_RES_FLAG_IO },
    { "pit", 	  0x40, 0x5f, ADI_RES_FLAG_IO },
    { "kbd", 	  0x60, 0x6f, ADI_RES_FLAG_IO },
    { "dma page", 0x80, 0x8f, ADI_RES_FLAG_IO },
    { "pic2", 	  0xa0, 0xbf, ADI_RES_FLAG_IO },
    { "dma2", 	  0xc0, 0xdf, ADI_RES_FLAG_IO },
    { NULL, }
};

int adi_register_resource(struct adi_resource_s *adi_resource)
{
    struct list_head *tmp;
    struct list_head *res_head;
    struct adi_resource_s *res;
    int ret = 0;
    spinlock_t *res_lock;
    u32 flags;
    
    if ((adi_resource->res_end - adi_resource->res_start) == 0) {
	printk(MOD_NAME "zero size resource adding probe\n");
	return -1;
    }
    
    if (adi_resource->res_flags & ADI_RES_FLAG_MEM) {
	res_head = &adi_resources_mem_list;
	res_lock = &adi_resources_mem_lock;
    } else {
	res_head = &adi_resources_io_list;
	res_lock = &adi_resources_io_lock;
    }
    
    spin_lock_irqsave(res_lock, flags);
    
    if (!list_empty(res_head)) {   
	list_for_each(tmp, res_head) {
	    res = list_entry(tmp, struct adi_resource_s, res_list);

	    if ((res->res_start > adi_resource->res_start) && (res->res_start <= adi_resource->res_end)) {
		ret = -1;
		goto register_end;
	    }
	    if ((res->res_end >= adi_resource->res_start) && (res->res_end < adi_resource->res_end)) {
		ret = -1;
		goto register_end;
	    }

	    if (res->res_start > adi_resource->res_end) {
		__list_add(&adi_resource->res_list, res->res_list.prev, &res->res_list);
		ret = 0;
		goto register_end;
    	    }
	}
    } else {
	list_add(&(adi_resource->res_list), res_head);
	ret = 0;
	goto register_end;
    }
    
    list_add_tail(&(adi_resource->res_list), res_head);
        
register_end:
    spin_unlock_irqrestore(res_lock, flags);

    return ret;
}

int adi_unregister_resource(struct adi_resource_s *adi_resource)
{
    return 0;
}

int adi_check_resource(struct adi_resource_s *adi_resource)
{
    return 0;
}

int adi_print_resources(int res_flags_mask)
{
    struct list_head *tmp;
    struct list_head *res_head;
    struct adi_resource_s *res;
    spinlock_t *res_lock;
    u32 flags;

    if (res_flags_mask & ADI_RES_FLAG_MEM) {
	res_head = &adi_resources_mem_list;
	res_lock = &adi_resources_mem_lock;
	printk(MOD_NAME "ADI memory resources: \n");
    } else {
	res_head = &adi_resources_io_list;
	res_lock = &adi_resources_io_lock;
	printk(MOD_NAME "ADI io resources: \n");
    }

    spin_lock_irqsave(res_lock, flags);

    list_for_each(tmp, res_head) {
	res = list_entry(tmp, struct adi_resource_s, res_list);
	
	if (res_flags_mask & ADI_RES_FLAG_MEM)
	    printk(MOD_NAME "%08x-%08x %s\n", res->res_start, res->res_end, res->res_name);
	else
	    printk(MOD_NAME "%04x-%04x %s\n", res->res_start, res->res_end, res->res_name);
    }
	
    spin_unlock_irqrestore(res_lock, flags);

    return 0;
}

int adi_print_mem_resources(void)
{
    return adi_print_resources(ADI_RES_FLAG_MEM);
}

int adi_print_io_resources(void)
{
    return adi_print_resources(ADI_RES_FLAG_IO);
}

int adi_register_global_resources(void)
{
    int i = 0;
    int ret;

    while (global_io_resources[i].res_name != NULL) {
	ret = adi_register_resource(&global_io_resources[i]);	
	if (ret < 0)
	    return ret;
	    
    	i++;
    }
    
    return 0;
}

int adi_resources_init(void)
{
    printk(MOD_NAME "initializing adi resources\n");
    
    spin_lock_init(&adi_resources_io_lock);
    spin_lock_init(&adi_resources_mem_lock);

    adi_register_global_resources();

    return 0;
}
