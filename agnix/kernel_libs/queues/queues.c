/*
 * kernel_libs/queues/queues.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/05
 * Author: 	Lukasz Dembinski
 * Info:	queues.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/memory.h>
#include <agnix/queues.h>
#include <agnix/spinlock.h>
#include <agnix/resources.h>
#include <agnix/console.h>
#include <asm/cpu_ops.h>

#define QUEUES_DEBUG	0
#define MOD_NAME	"QUEUES: "

struct queue_type_s *queue_types[MAX_QUEUE_TYPES];
struct queue_s      queues[MAX_QUEUES];

u32    queue_bitmap[QUEUE_BITMAP_LEN];

int queues_resource_desc;
struct resource_s queues_resource = {
    .resource_name	= "queues",
    .resource_bitmap	= queue_bitmap,
    .resource_len	= QUEUE_BITMAP_LEN,
};

int queue_enqueue(int queue_desc, struct queue_entry_s *queue_entry)
{
    struct queue_s *queue = &queues[queue_desc];
    u32 flags;

    spin_lock_irqsave(&queue->queue_lock, flags);
    queue->queue_ops->enqueue(queue, queue_entry);
    spin_unlock_irqrestore(&queue->queue_lock, flags);

    return 0;
}

struct queue_entry_s *queue_dequeue(int queue_desc)
{
    struct queue_entry_s *queue_entry;
    struct queue_s *queue = &queues[queue_desc];
    u32 flags;

    spin_lock_irqsave(&queue->queue_lock, flags);
    queue_entry = queue->queue_ops->dequeue(queue);
    spin_unlock_irqrestore(&queue->queue_lock, flags);

    return queue_entry;
}

int put_free_queue(int queue_desc)
{
    return put_free_resource(queues_resource_desc, queue_desc);
}

int get_free_queue(void)
{
    return get_free_resource(queues_resource_desc);
}

/*
 * register_queue()
 *
 *       : register the queue of type queue_type and return descriptor to the queue
 * return: -1 if the queue of type is not registered or there is no space for a new queue_type
 *          0 if OK
 */
int register_queue(int queue_type, const char *queue_name)
{
    struct queue_s *queue;
    int queue_desc = 0;
    
    if (queue_types[queue_type] == NULL) {
	printk(MOD_NAME "queue type %d not registered...\n", queue_type);
	return -1;
    }
    
    if ((queue_desc = get_free_queue()) < 0) {
	printk(MOD_NAME "can not register %s queue, type %d\n", queue_name, queue_type);
	return -1;
    }
#if QUEUES_DEBUG
    else
        printk(MOD_NAME "registering %s queue, type %s\n", queue_name, queue_types[queue_type]->queue_type_name);
#endif

    queue = &queues[queue_type];
    queue->queue_name = queue_name;
    queue->queue_type = queue_type;
    queue->queue_ops  = queue_types[queue_type]->queue_type_ops->queue_ops;
    INIT_LIST_HEAD(&queue->queue_list);

    if (queue->queue_ops && queue->queue_ops->init)
	queue->queue_ops->init(queue);

    atomic_inc(&queue_types[queue_type]->queue_type_count);

    return queue_desc;
}

/* pozmieniac kolejnosc wykonywania - kolejka moze miec jakies dane jeszcze */
int unregister_queue(int queue_desc)
{
    struct queue_s *queue;
    const char *queue_name;
    int queue_type;
    int ret;

    queue = &queues[queue_desc];

    queue_name = queue->queue_name;
    queue_type = queue->queue_type;

    if ((ret = put_free_queue(queue_desc)) < 0) {
	printk(MOD_NAME "can not unregister %s queue, type %d\n", queue_name, queue_type);
	return -1;
    }
#if QUEUES_DEBUG
    else
	printk(MOD_NAME "unregistering %s queue, type %s\n", queue_name, queue_types[queue_type]->queue_type_name);
#endif

    if (queue->queue_ops && queue->queue_ops->remove)
	queue->queue_ops->remove(queue);

    atomic_dec(&queue_types[queue_type]->queue_type_count);

    return 0;
}

/*
 * register_queue_type()
 *
 *       : register queue type 
 * return: -1 if pointer to a queue type is NULL or a queue type is already registered
 *          0 if OK
 */
int register_queue_type(struct queue_type_s *queue)
{
    struct queue_type_ops_s *queue_type_ops;
    int queue_type;
    int ret = 0;

    if (queue == NULL) {
	printk(MOD_NAME "can not register queue_type with NULL pointer...\n");
	return -1;
    }

    queue_type = queue->queue_type;

    if (queue_types[queue_type]) {
	printk(MOD_NAME "queue_type %d already registered...\n", queue_type);
	return -1;
    }
#if QUEUES_DEBUG
    else
        printk(MOD_NAME "registering %s queue type (%d)\n", queue->queue_type_name, queue_type);
#endif

    queue_types[queue_type] = queue;
    queue_type_ops = queue->queue_type_ops;

    if (queue_type_ops && queue_type_ops->init) {
	ret = queue_type_ops->init(queue);
    }

    return ret;
}

/*
 * unregister_queue_type()
 *
 *       : unregister queue of type queue_type
 * return: -1 if a queue type is not registered or is already in use
 *          0 if OK
 */
int unregister_queue_type(int queue_type)
{
    struct queue_type_s *queue;
    struct queue_type_ops_s *queue_type_ops;
    int ret = 0;

    queue = queue_types[queue_type];

    if (queue == NULL) {
	printk(MOD_NAME "queue type %d not registered...\n", queue_type);
	return -1;
    }

    if (atomic_read(&queue->queue_type_count) == 0) {

#if QUEUES_DEBUG
        printk(MOD_NAME "unregistering %s queue type (%d)\n", queue->queue_type_name, queue_type);
#endif
        queue_type_ops = queue->queue_type_ops;
        if (queue_type_ops && queue_type_ops->remove) {
	    ret = queue_type_ops->remove(queue);
	}
    } else {
    	printk(MOD_NAME "queue type %d already in use...\n", queue_type);
	ret = -1;
    }

    return ret;
}

/*
 * queues_init()
 *
 *       : initializing all queue types
 * return: always 0
 */
int queues_init(void)
{
    int i;

    printk(MOD_NAME "initializing queues\n");

    queues_resource_desc = register_resource(&queues_resource);
    memset(queue_types, 0, MAX_QUEUE_TYPES * sizeof(struct queue_type_s));

    for (i = 0; i < MAX_QUEUES; i++) {
	INIT_LIST_HEAD(&queues[i].queue_list);
	spin_lock_init(&queues[i].queue_lock);
    }

    queues_fifo_init();
    queues_lifo_init();

    return 0;
}
