/*
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	init.c file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

//#define __KERNEL__

#include <agnix/config.h>
#include <agnix/sched.h>
#include <agnix/console.h>
#include <agnix/ioport.h>
#include <agnix/tasks.h> /* tmp */
#include <asm/types.h> /* tmp */
#include <asm/irq.h>
#include <agnix/memory.h> /* tmp */
#include <agnix/adi/adi.h>
//#include <agnix/chips.h>
#include <agnix/crypt.h>

#include <agnix/syscalls.h>

#include <agnix/unistd.h>

//#include <agnix/fastrand.h>
//#include <agnix/timer.h>
//#include <agnix/math64.h>
//#include <agnix/hashing.h>

#include <agnix/threads.h>

#include <agnix/queues.h>

#include <asm/memtests.h>
#include <agnix/counters.h>

#include <agnix/timers.h>
#include <agnix/terminal.h>

int errno;

extern char *agnix_banner;
extern char *agnix_version;
extern char *agnix_license;

extern int adi_console_init(void);
extern int      i386_init(void);
extern int      mtrr_init(void);
extern int    printk_init(void);
//extern int  memalloc_init(void);
extern int scheduler_init(void);
extern int     tasks_init(void);
extern int        pm_init(void);
extern int    timers_init(void);
extern int	fork_init(void);
extern int data_structures_init(void);
extern int    queues_init(void);
extern int    bootmem_free_init(void);
extern int	resources_init(void);
extern int	counters_init(void);

extern struct console_s kernel_console;
extern int count;
//extern u32 jiffies_64[2];

void init_thread(void *data)
{
    int i;
    int count = 0;
    struct timeval_s my_tv;    

    __sti();

    for (;;) {
        timer_gettimeofday(&my_tv);
	for (i = 0; i < 10000000; i++);

	count++;
    }
    
}

void init_start(void)
{
    create_kernel_thread("init", init_thread, NULL);
}

int clean_kernel(void)
{
    printk("Cleaning kernel ...\n");
    
    bootmem_free_init();

    return 0;
}

int queue_test(void)
{
    struct queue_entry_s entry1, entry2, entry3;
    struct queue_entry_s *entry;
    char *dane = "ala ma kotka";
    char *dane2 = "lukasz ma pieska";
    char *dane3 = "aga ma rybki";
    int queue_desc, queue_desc2;

    entry1.queue_entry_data = dane;
    entry2.queue_entry_data = dane2;
    entry3.queue_entry_data = dane3;

    queue_desc = register_queue(QUEUE_TYPE_FIFO, "myfifo");
    printk("QUEUE_DESC = %d\n", queue_desc);

    queue_enqueue(queue_desc, &entry1);    
    queue_enqueue(queue_desc, &entry2);    
    queue_enqueue(queue_desc, &entry3);    

    entry = queue_dequeue(queue_desc);
    printk("HHEE %s\n", entry->queue_entry_data);

    entry = queue_dequeue(queue_desc);
    printk("HHEE %s\n", entry->queue_entry_data);

    entry = queue_dequeue(queue_desc);
    printk("HHEE %s\n", entry->queue_entry_data);

    if ((entry = queue_dequeue(queue_desc)) != NULL)
        printk("HHEE %s\n", entry->queue_entry_data);
    else
	printk("HHEE NULL\n");
	
    queue_desc2 = register_queue(QUEUE_TYPE_LIFO, "mylifo");
    printk("QUEUE_DESC = %d\n", queue_desc2);

    queue_enqueue(queue_desc2, &entry1);    
    queue_enqueue(queue_desc2, &entry2);    
    queue_enqueue(queue_desc2, &entry3);    

    entry = queue_dequeue(queue_desc2);
    printk("HHEE %s\n", entry->queue_entry_data);

    entry = queue_dequeue(queue_desc2);
    printk("HHEE %s\n", entry->queue_entry_data);

    entry = queue_dequeue(queue_desc2);
    printk("HHEE %s\n", entry->queue_entry_data);

    if ((entry = queue_dequeue(queue_desc2)) != NULL)
        printk("HHEE %s\n", entry->queue_entry_data);
    else
	printk("HHEE NULL\n");

    unregister_queue(queue_desc);
    unregister_queue(queue_desc2);

    return 0;
}

void probe_timer_fn(void *data);
void probe_timer_fn2(void *data);

struct timer_s probe_timer = {
    .timer_expire = 1713,
    .timer_function = probe_timer_fn,
};

struct timer_s probe_timer2 = {
    .timer_expire = 15000,
    .timer_function = probe_timer_fn2,
    .timer_flags = TIMER_FLAG_ONESHOT
};

void probe_timer_fn(void *data)
{
    printk("Timer expire\n");
}

void probe_timer_fn2(void *data)
{
    printk("Timer expire 2\n");
}

struct itimerval_s timerval = {
};

void start_kernel(void)
{
    int i, k;
    u32 ptr;

    terminals_init();
#if CONFIG_DRIVERS_CHAR
    adi_console_init();
#endif
    memalloc_init();
    resources_init();
    counters_init();

    data_structures_init();
    queues_init();
    adi_chips_init();
    i386_init();
#if CONFIG_MEMTESTS
    do_memtests();
#endif
    adi_drivers_init();
    fork_init();
    tasks_init();
    scheduler_init();

    __sti();
    cpu_calibrate_delay();
    __cli();

    timers_init();
    crypt_init();
    init_start();
    pm_init();
    clean_kernel();

    printk(NEW_LINE);
    printk(agnix_banner);
    printk(agnix_version);
    printk(agnix_license);
    printk(NEW_LINE);

    __sti();

//    timerval.it_value.tv_sec = 1;
//    timerval.it_interval.tv_sec = 3;
//    do_sys_setitimer(0, &timerval, NULL);

//    register_timer(&probe_timer);
//    register_timer(&probe_timer2);

//    queue_test();

//    kill(2, 10);

//    k = 0;
//    for (;;) {
///	ptr = get_free_pages(0);
//	printk("%x\n\n", ptr);
//	buddy_alloc_print_units();
//	for(i = 0; i < 200000000; i++);
//    }
//
//    __sti();

    for(;;);
//	idle_state_enter();
}
