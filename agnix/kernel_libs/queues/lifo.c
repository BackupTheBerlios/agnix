/*
 * kernel_libs/queues/lifo.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/05
 * Author: 	Lukasz Dembinski
 * Info:	lifo.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/queues.h>
#include <agnix/list.h>
#include <agnix/console.h>

#define MOD_NAME	"QUEUES: \t"

int queue_type_lifo_init(struct queue_type_s *queue_type)
{
    return 0;
}

int queue_type_lifo_remove(struct queue_type_s *queue_type)
{
    return 0;
}

int queue_lifo_init(struct queue_s *queue)
{
    return 0;
}

int queue_lifo_remove(struct queue_s *queue)
{
    return 0;
}

int queue_lifo_enqueue(struct queue_s *queue, struct queue_entry_s *queue_entry)
{
    list_add(&queue_entry->queue_entry_list, &queue->queue_list);    

    return 0;
}

struct queue_entry_s *queue_lifo_dequeue(struct queue_s *queue)
{
    struct queue_entry_s *queue_entry;

    if (list_empty(&queue->queue_list))
	return NULL;

    queue_entry = list_entry(queue->queue_list.next, struct queue_entry_s, queue_entry_list);

    list_del(queue->queue_list.next);    

    return queue_entry;
}

int queue_lifo_control(struct queue_s *queue, int op, void *arg)
{
    return 0;
}

struct queue_ops_s queue_lifo_ops = {
    .init		= queue_lifo_init,
    .remove		= queue_lifo_remove,
    .enqueue		= queue_lifo_enqueue,
    .dequeue		= queue_lifo_dequeue,
    .control		= queue_lifo_control,
    .peek		= NULL
};

struct queue_type_ops_s queue_type_lifo_ops = {
    .init		= queue_type_lifo_init,
    .remove		= queue_type_lifo_remove,
    .queue_ops		= &queue_lifo_ops
};

struct queue_type_s queue_type_lifo = {
    .queue_type		= QUEUE_TYPE_LIFO,
    .queue_type_name	= "LIFO",
    .queue_type_ops	= &queue_type_lifo_ops,
};

int queues_lifo_init(void)
{
    return register_queue_type(&queue_type_lifo);
}
