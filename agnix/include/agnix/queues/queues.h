#ifndef _AGNIX_QUEUES_QUEUES_H_
#define _AGNIX_QUEUES_QUEUES_H_

#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <asm/cpu_ops.h>

#define MAX_QUEUE_TYPES		16
#define MAX_QUEUES		32
#define MAX_QUEUE_NAME_LEN	32
#define QUEUE_BITMAP_LEN	(((MAX_QUEUES - 1) >> 5) + 1)

#define QUEUE_TYPE_FIFO		0x00
#define QUEUE_TYPE_LIFO		0x01
#define QUEUE_TYPE_LPF		0x02

struct queue_type_ops_s;

struct queue_type_s {
    int 			queue_type;
    const char *		queue_type_name;
    struct queue_type_ops_s *	queue_type_ops;
    u32				queue_type_count;
};

struct queue_entry_s {	
    struct list_head 	queue_entry_list;
    void *		queue_entry_data;
};

struct queue_params_s {
    int queue_max_len;
};

struct queue_s {
    const char *	    queue_name;
    int 	     	    queue_type;
    struct queue_ops_s *    queue_ops;
    spinlock_t 	     	    queue_lock;
    struct list_head 	    queue_list;
    struct queue_params_s * queue_params;
};

struct queue_ops_s {
    int 		   (*init)   (struct queue_s *queue);
    int 		   (*remove) (struct queue_s *queue);
    int 		   (*enqueue)(struct queue_s *queue, struct queue_entry_s *queue_entry);
    struct queue_entry_s * (*dequeue)(struct queue_s *queue);
    int			   (*control)(struct queue_s *queue, int op, void *arg);
    struct queue_entry_s * (*peek)(struct queue_s *queue);
};

struct queue_type_ops_s {
    int 		   (*init)  (struct queue_type_s *queue_type);
    int 		   (*remove)(struct queue_type_s *queue_type);
    struct queue_ops_s	   *queue_ops;
};

int register_queue_type	 (struct queue_type_s *queue_type);
int unregister_queue_type(int queue_type);
int register_queue	 (int queue_type, const char *queue_name);
int unregister_queue	 (int queue_desc);
int queue_init		 (int queue_desc);
int queue_remove         (int queue_desc);
int queue_enqueue	 (int queue_desc, struct queue_entry_s *);
struct queue_entry_s *	 queue_dequeue(int queue_desc);

int queues_init(void);

#include <agnix/queues/fifo.h>
#include <agnix/queues/lifo.h>

#endif
