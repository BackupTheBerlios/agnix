/*
 * kernel_libs/data_structures/hash_st.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	hash_st.c core file, hashing with grouping indexes
 * Contact:	mailto: <dembol@linux.net>
 * 
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/memory.h>
#include <agnix/list.h>
#include <agnix/errno.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>

#define MOD_NAME		"HASH_ST: "

#define HASH_ENTRIES		((PAGE_SIZE - sizeof(struct hash_table_s)) >> 3)
#define MAX_HASH_TABLES		16
#define MAX_HASH_ORDER		4
#define FREE_PSEUDO_KEY		~0

#define find_hash_table(hashd) (&hash_tables[hashd])

struct hash_entry_s {
    u32 hash_key;
    u32 hash_addr;
};

struct hash_table_s {
    spinlock_t hash_lock;
    int max_entries;
    int cur_entries;
    int page_order;
    int hashd;

    u32 (*hash_fn)(u32 key);
//    struct list_head hash_list;
    struct hash_entry_s *hash_entry;
};

struct hash_table_s hash_tables[MAX_HASH_TABLES];

/* this routine math a pseudo key */
static u32 hash_fn_default(u32 key)
{
    return key;
}

static u32 hash_math_pseudo(struct hash_table_s *hash_table_ptr, u32 hash_key)
{
    if (hash_table_ptr->hash_fn)
	return hash_table_ptr->hash_fn(hash_key);
    else
	return hash_key;
}

void hash_count_entries(int hashd) 
{
    struct hash_table_s *hash_table_ptr;    

    hash_table_ptr = find_hash_table(hashd);
    printk("hash_ptr %x\n", hash_table_ptr);
    printk("hash_max_entries %x\n", hash_table_ptr->max_entries);
    printk("hash_cur_entries %x\n", hash_table_ptr->cur_entries);
}

int hash_entry_add(int hashd, u32 hash_key, u32 hash_addr)
{
    struct hash_table_s *hash_table_ptr;
    struct hash_entry_s *hash_entry;
    u32 pseudo_key = 0;
    int loops;

    spin_lock(&hash_table_ptr->hash_lock);

    hash_table_ptr = find_hash_table(hashd);

    if (!hash_table_ptr->max_entries)
	return -1;

    hash_entry = hash_table_ptr->hash_entry;
    pseudo_key = hash_math_pseudo(hash_table_ptr, hash_key) % hash_table_ptr->max_entries;

    loops = 0;
    do {
	if (hash_entry[pseudo_key].hash_key != FREE_PSEUDO_KEY) {
	    pseudo_key = (pseudo_key + 1) % hash_table_ptr->max_entries;
	} else {
	    hash_entry[pseudo_key].hash_key = hash_key;
            hash_entry[pseudo_key].hash_addr = hash_addr;
	    hash_table_ptr->cur_entries++;
	    spin_unlock(&hash_table_ptr->hash_lock);
	    return 0;
	}
	
	loops++;
	
    } while(loops < hash_table_ptr->max_entries);
        
    spin_unlock(&hash_table_ptr->hash_lock);
    
    return -1;   
}

/* this function shouldn't be called from irq */
u32 hash_entry_find(int hashd, u32 hash_key)
{
    struct hash_table_s *hash_table_ptr;
    struct hash_entry_s *hash_entry;
    u32 pseudo_key = 0;
    int loops;
    
    spin_lock(&hash_table_ptr->hash_lock);
    
    hash_table_ptr = find_hash_table(hashd);
    if (!hash_table_ptr->max_entries)
	return -1;

    hash_entry = hash_table_ptr->hash_entry;
    pseudo_key = hash_math_pseudo(hash_table_ptr, hash_key) % hash_table_ptr->max_entries;

    loops = 0;    
    do {
	if (hash_entry[pseudo_key].hash_key != hash_key)
	    pseudo_key = (pseudo_key + 1) % hash_table_ptr->max_entries;
	else {   
	
	    spin_unlock(&hash_table_ptr->hash_lock);
	    return hash_entry[pseudo_key].hash_addr;
	}
	
	loops++;
	
    } while(loops < hash_table_ptr->max_entries);
    
    spin_unlock(&hash_table_ptr->hash_lock);
    
    return 0;
}

static int get_empty_hash_table(void)
{
    int i;
    
    for (i = 0; i < MAX_HASH_TABLES; i++) {
	spin_lock(&hash_tables[i].hash_lock);
	if (!hash_tables[i].cur_entries &&
	    !hash_tables[i].max_entries) {
		spin_unlock(&hash_tables[i].hash_lock);
		return i;
	}
	    
	spin_unlock(&hash_tables[i].hash_lock);
    }
    
    return -1;
}

static void put_empty_hash_table(int hashd)
{
    spin_lock(&hash_tables[hashd].hash_lock);
    memset(&hash_tables[hashd], 0, sizeof(struct hash_table_s));
    spin_unlock(&hash_tables[hashd].hash_lock);
}

int register_hash_table(u32 rows, u32 (*hash_fn)(u32 key))
{
    struct hash_table_s *hash_table;
    struct hash_entry_s *hash_entry;
    int page_order = 0;  
    int hashd;  
    int i;

    hashd = get_empty_hash_table();
    if (hashd == -1)
	return -EAGAIN;

    for (i = 0; i < MAX_HASH_ORDER; i++) {
        if (rows < (((1 << i) * PAGE_SIZE) / sizeof(struct hash_entry_s))) {
	    page_order = i;
	    break;
	}
    }

    if (i == MAX_HASH_ORDER)
	return -EINVAL;

    /* this will be done by slab allocator, but we have no it :( */
    hash_table = &hash_tables[hashd];
    hash_entry = (struct hash_entry_s *)get_free_pages(page_order);
    
    if (!hash_table) {
	put_empty_hash_table(hashd);
	return -ENOMEM;
    }

    if (hash_fn)
	hash_table->hash_fn = hash_fn;
    else
	hash_table->hash_fn = hash_fn_default;

    hash_table->max_entries = ((1 << page_order) * PAGE_SIZE) / sizeof(struct hash_entry_s);
    hash_table->cur_entries = 0;
    hash_table->page_order  = page_order;
    hash_table->hashd       = hashd;        
    hash_table->hash_entry  = hash_entry;
    memset((void *)(hash_table->hash_entry), 0xFF, (1 << page_order) * PAGE_SIZE);
//	   hash_table->max_entries * sizeof(struct hash_entry_s));

    printk(MOD_NAME "registering hashing table (%d rows/%d pages)\n", hash_table->max_entries, page_order);

    spin_lock_init(&hash_table->hash_lock);
    
    return hashd;
}

int unregister_hash_table(int hashd)
{
    struct hash_table_s *hash_table_ptr;    

    hash_table_ptr = find_hash_table(hashd);

    if (hash_table_ptr) {
        put_free_pages((u32)hash_table_ptr->hash_entry, (u8)(hash_table_ptr->page_order));
	put_free_pages((u32)hash_table_ptr, 0);
    }

    return 0;
}

int hash_static_init(void)
{
    int i;

    for (i = 0; i < MAX_HASH_TABLES; i++) {
	memset((void *)&hash_tables[i], 0, sizeof(struct hash_table_s));
//    	hash_tables[i].max_entries = 0;
//	hash_tables[i].cur_entries = 0;
    }

//    spin_lock_init(&hash_list_lock);

    return 0;
}

