/*
 * kernel_libs/queues/fifo.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/05
 * Author: 	Lukasz Dembinski
 * Info:	fifo.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/queues.h>
#include <agnix/list.h>
#include <agnix/console.h>

#define MOD_NAME	"QUEUES: \t"

int queue_type_fifo_init(struct queue_type_s *queue_type)
{
    return 0;
}

int queue_type_fifo_remove(struct queue_type_s *queue_type)
{
    return 0;
}

int queue_fifo_init(struct queue_s *queue)
{
    return 0;
}

int queue_fifo_remove(struct queue_s *queue)
{
    return 0;
}

int queue_fifo_enqueue(struct queue_s *queue, struct queue_entry_s *queue_entry)
{
    list_add_tail(&queue_entry->queue_entry_list, &queue->queue_list);    

    return 0;
}

struct queue_entry_s *queue_fifo_dequeue(struct queue_s *queue)
{
    struct queue_entry_s *queue_entry;

    if (list_empty(&queue->queue_list))
	return NULL;

    queue_entry = list_entry(queue->queue_list.next, struct queue_entry_s, queue_entry_list);

    list_del(queue->queue_list.next);    

    return queue_entry;
}

int queue_fifo_control(struct queue_s *queue, int op, void *arg)
{
    return 0;
}

struct queue_ops_s queue_fifo_ops = {
    .init		= queue_fifo_init,
    .remove		= queue_fifo_remove,
    .enqueue		= queue_fifo_enqueue,
    .dequeue		= queue_fifo_dequeue,
    .control		= queue_fifo_control,
    .peek		= NULL
};

struct queue_type_ops_s queue_type_fifo_ops = {
    .init		= queue_type_fifo_init,
    .remove		= queue_type_fifo_remove,
    .queue_ops		= &queue_fifo_ops
};

struct queue_type_s queue_type_fifo = {
    .queue_type		= QUEUE_TYPE_FIFO,
    .queue_type_name	= "FIFO",
    .queue_type_ops	= &queue_type_fifo_ops,
};

int queues_fifo_init(void)
{
    return register_queue_type(&queue_type_fifo);
}
