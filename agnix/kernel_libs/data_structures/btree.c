/*
 * kernel_libs/data_structures/btree.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/05
 * Author: 	Lukasz Dembinski
 * Info:	btree.c core file
 * Contact:	mailto: <dembol@linux.net>
 * Status:	started
 */

#include <agnix/agnix.h>
#include <agnix/resources.h>
#include <agnix/memory.h>
#include <agnix/spinlock.h>
#include <agnix/list.h>
#include <agnix/console.h>

#define MOD_NAME	"BTREE: "
#define MAX_BTREES	16

#define MAX_BTREE_NODE_RATIO		16

#define BTREE_NODE_ENTRY_PRESENT	0x01
#define BTREE_NODE_ENTRY_EMPTY		0x02
#define BTREE_NODE_ENTRY_END		0x04
#define BTREE_NODE_ENTRY_LEAF		0x08

#define BTREE_NODE_ROOT			0x10
#define BTREE_NODE_MIDDLE		0x20

#define BTREE_SEARCH_FOUND		0
#define BTREE_SEARCH_NOT_EXISTS		-1
#define BTREE_SEARCH_ROOT_NOT_EXISTS	-2

#define BTREE_INSERT_OK			0
#define BTREE_INSERT_EXISTS		-1

struct btree_ops_s;

struct btree_s {
    const char *	 btree_name;
    int 		 btree_ratio;
    struct list_head 	 btree_root_list;    
    spinlock_t 		 btree_lock;
    struct btree_ops_s * btree_ops;
};

struct btree_ops_s {
        
};

struct btree_node_entry_s {
    struct list_head	node_entry_list;
    int			node_entry_status;
    u32 		node_entry_key;
    u32 		node_entry_address;
};

struct btree_node_s {
    struct btree_node_entry_s *	node_entries;
    int		 		node_entries_count;
    int		 		node_ratio;
    struct list_head		node_list;
};

struct btree_s btrees[MAX_BTREES];

u32 btree_bitmap[MAX_BTREES];

int btree_resource_desc;
struct resource_s btree_resource = {
    .resource_name	= "btree",
    .resource_bitmap	= btree_bitmap,
    .resource_len	= MAX_BTREES,
};

struct btree_node_s *btree_alloc_node(int node_ratio)
{
    struct btree_node_s *node_alloc;
    struct btree_node_entry_s *node_entries_alloc;
    
    if (node_ratio > MAX_BTREE_NODE_RATIO) {
	printk(MOD_NAME "btree_node_ratio == MAX_BTREE_NODE_RATIO\n");
	return NULL;
    }
    
    node_alloc = (struct btree_node_s *)get_free_pages(0);
    
    if (node_alloc == NULL) {
	printk(MOD_NAME "btree_alloc_node failed, node_alloc == NULL\n");
	return NULL;
    }
    
    node_entries_alloc = (struct btree_node_entry_s *)get_free_pages(0);

    if (node_alloc == NULL) {
	printk(MOD_NAME "btree_alloc_node failed, node_alloc == NULL\n");
	goto btree_free_node;
    }

    node_alloc->node_entries_count 	= 1; /* end pointer */
    node_alloc->node_list.next 		= node_alloc->node_list.prev = NULL;
    node_alloc->node_ratio 		= node_ratio;
    node_alloc->node_entries 		= node_entries_alloc;
    
    node_entries_alloc[0].node_entry_status = BTREE_NODE_ENTRY_PRESENT | BTREE_NODE_ENTRY_END | 
                                              BTREE_NODE_ENTRY_LEAF;
    
    return node_alloc;
    
btree_free_node:
    put_free_pages((u32)node_alloc, 0);
    return NULL;
}

int btree_search_lock(struct btree_s *btree, u32 key, struct btree_node_s **node_parent, 
		      struct btree_node_s **node_found, int *key_position);

int btree_insert_overflow_lock(struct btree_node_s *node_found, struct btree_node_s *node_parent, u32 key, 
			       u32 address, int key_position)
{
    struct btree_node_s *node_neigh_left;
    struct btree_node_s *node_neigh_right;
    int i;

    if (node_parent == NULL)
    {
	printk("Splitting root\n");
	return 0;
    }

    for (i = 0; i < node_parent->node_entries_count; i++) {
	if ((node_parent->node_entries[i].node_entry_key < key) || 
	    (node_parent->node_entries[i].node_entry_status & BTREE_NODE_ENTRY_END))
	    break;
    }

    node_neigh_right = list_entry(node_parent->node_entries[i].node_entry_list.next, struct btree_node_s, node_list);
    if (i + 1 < node_parent->node_entries_count) 
        node_neigh_left = list_entry(node_parent->node_entries[i + 1].node_entry_list.next, struct btree_node_s, node_list);

    return 0;
}

int btree_insert_entry_lock(struct btree_node_s *node, u32 key, u32 address, int key_position)
{
    struct btree_node_entry_s *entry = node->node_entries;
    int i;
    
//    for (i = (node->node_ratio << 1) - 1; i > key_position; i--) {
    for (i = node->node_entries_count; i > key_position; i--) {
	memcpy(&entry[i], &entry[i - 1], sizeof(struct btree_node_entry_s));
    }
    
    entry[key_position].node_entry_list.next 	= NULL;
    entry[key_position].node_entry_list.prev 	= NULL; /* unused */
    entry[key_position].node_entry_key 		= key;
    entry[key_position].node_entry_address 	= address;
    entry[key_position].node_entry_status 	= BTREE_NODE_ENTRY_PRESENT | BTREE_NODE_ENTRY_LEAF;
    
    return BTREE_INSERT_OK;
}

int btree_insert_lock(struct btree_s *btree, u32 key, u32 address)
{
    struct btree_node_s *node_found;
    struct btree_node_s *node_parent;
    int key_position;
    int ret;
    
    ret = btree_search_lock(btree, key, &node_parent, &node_found, &key_position);
    
    if (ret == BTREE_SEARCH_FOUND)
	return BTREE_INSERT_EXISTS;
    else
    if (ret == BTREE_SEARCH_ROOT_NOT_EXISTS) {
	/* we have to alloc root node if it doesn't exists */
	node_parent  = NULL;
	node_found   = btree_alloc_node(btree->btree_ratio);
	key_position = 0;
	btree->btree_root_list.next = &node_found->node_list;
    }

    if ((node_found->node_ratio << 1) > node_found->node_entries_count)
	return btree_insert_entry_lock(node_found, key, address, key_position);

    btree_insert_overflow_lock(node_found, node_parent, key, address, key_position);

    return BTREE_INSERT_OK;
}

int btree_insert(int btree_desc, u32 key, u32 address)
{
    struct btree_s *btree = &btrees[btree_desc];
    u32 flags;
    int ret;

    if (!btree) {
	printk(MOD_NAME "btree pointer is NULL\n");
	return -1;
    }

    spin_lock_irqsave(&btree->btree_lock, flags);
    ret = btree_insert_lock(btree, key, address);
    spin_unlock_irqrestore(&btree->btree_lock, flags);

    return ret;
}

int btree_search_node_lock(struct btree_node_s *node, u32 key, struct btree_node_s **node_parent, 
			   struct btree_node_s **node_found, int *key_position)
{
    struct btree_node_s *node_child = NULL;
    struct btree_node_entry_s *node_entry;
    int i;

    *node_parent = NULL;

    if (node->node_entries == 0)
	return BTREE_SEARCH_NOT_EXISTS;

    for (i = 0; i < node->node_entries_count; i++) {

	node_entry = &node->node_entries[i];
	if (node_entry->node_entry_status & BTREE_NODE_ENTRY_PRESENT) {
	
	    if (node_entry->node_entry_key < key || 
	       (node_entry->node_entry_status & BTREE_NODE_ENTRY_END)) {
	
		if (node_entry->node_entry_list.next == NULL) {
		    *key_position = i;
		    *node_found = node;
		    return BTREE_SEARCH_NOT_EXISTS;
		}
	
		node_child = list_entry(node_entry->node_entry_list.next, 
					  struct btree_node_s, node_list);
		break;
	    }
	
    	    if (node_entry->node_entry_key == key) {
		*key_position = i;
		*node_found = node;
		return BTREE_SEARCH_FOUND;
	    }
	} else 
	    return -1;
    }
    
    if (node_child != NULL) {
	*node_parent = node;
        return btree_search_node_lock(node_child, key, node_parent, node_found, key_position);
    } else {
	*key_position = i;
	*node_found = node;
	return BTREE_SEARCH_NOT_EXISTS;
    }
}

int btree_search_lock(struct btree_s *btree, u32 key, struct btree_node_s **node_parent, 
		      struct btree_node_s **node_found, int *key_position)
{
    struct btree_node_s *node;
    int ret;

    if (btree->btree_root_list.next == NULL)
	return BTREE_SEARCH_ROOT_NOT_EXISTS;
    
    node = list_entry(btree->btree_root_list.next, struct btree_node_s, node_list);

    ret = btree_search_node_lock(node, key, node_parent, node_found, key_position);

    return ret;
}

u32 btree_search(int btree_desc, u32 key)
{
    struct btree_s *btree = &btrees[btree_desc];
    struct btree_node_s *node_found;
    struct btree_node_s *node_parent;
    int key_position = -1;
    int ret;
    u32 flags;

    if (!btree) {
	printk(MOD_NAME "btree pointer is NULL\n");
	return 0;
    }

    spin_lock_irqsave(&btree->btree_lock, flags);
    ret = btree_search_lock(btree, key, &node_parent, &node_found, &key_position);
    spin_unlock_irqrestore(&btree->btree_lock, flags);

    if (ret == BTREE_SEARCH_FOUND)
	    return node_found->node_entries[key_position].node_entry_address;
    
    return 0;
}

int put_free_btree(int btree_desc)
{
    return put_free_resource(btree_resource_desc, btree_desc);
}

int get_free_btree(void)
{
    return get_free_resource(btree_resource_desc);
}

int register_btree(int btree_ratio, const char *btree_name)
{
    int btree_desc;

    btree_desc = get_free_btree();
    
    if (btree_desc < 0) {
	printk(MOD_NAME "register_btree failed, there is no free resources\n");
	return -1;
    }

    printk(MOD_NAME "registering %s btree, btree_ratio = %d\n", btree_name, btree_ratio);

    memset(&btrees[btree_desc], 0, sizeof(struct btree_s));
    btrees[btree_desc].btree_name = btree_name;
    spin_lock_init(&btrees[btree_desc].btree_lock);

    return btree_desc;
}

int unregister_btree(int btree_desc)
{
    put_free_btree(btree_desc);

    printk(MOD_NAME "registering %s btree\n", btrees[btree_desc].btree_name);

    return 0;
}

int btree_init(void)
{
    printk(MOD_NAME "Initializing b-trees\n");

    memset(btrees, 0, sizeof(struct btree_s ) * MAX_BTREES);
    btree_resource_desc = register_resource(&btree_resource);

    return 0;
}
