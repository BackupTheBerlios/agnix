/*
 * kernel_sys/thread.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/04
 * Author: 	Lukasz Dembinski
 * Info:	thread.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <asm/tss.h>
#include <agnix/tasks.h>
#include <agnix/list.h>
#include <agnix/console.h>
#include <agnix/spinlock.h>
#include <agnix/panic.h>
#include <agnix/errno.h>
#include <agnix/unistd.h>
#include <agnix/linkage.h>

#define MOD_NAME	"THREAD: "

//int do_sys_fork(struct tss_wrap_s *tss_wrap);

int create_kernel_thread(const char *name, void (*thread_proc)(void *), void *data) 
{
    __asm__ __volatile__ (
	"movl %%esp, %%esi\n\t"
	"int $0x80\n\t"
	"cmpl %%esp, %%esi\n\t"
	"je 1f\n\t"
	
	"movl %3, %%eax\n\t"
	"pushl %%eax\n\t"
	"call *%2\n\t"
	"1:"
    :
    :"a"(__NR_fork), "b"(name), "r"(thread_proc), "r"(data)
    );

    // za 1 wstawic powiadomienie o tym, ze sie skonczyl thread
    
//    if (current_task->t_pid > 0) {
//	__sti();

/*	__asm__ __volatile__ (
	    "pushl %edx\n\t"
	    "call  %ecx\n\t"
	    :
	    :"c"(thread_proc), "d"(data)
	);
*/
//	__asm__ __volatile__ (
//	    ""
//	    :"=c"(ecx)
//	);

//	printk("%s %08x\n", name, ecx);
//	kernel_panic("Kernel thread terminated...\n");
//    } else 
//    if (current_task->t_pid < 0) {
//	kernel_panic(MOD_NAME "can not create kernel thread\n");
//    }

    return 0;
}

int create_user_thread(const char *name, void (*thread_proc)(void *data), void *data) 
{
    fork();
    
    if (current_task->t_pid > 0) {
//    	task_set_user_mode(&(current_thread->tss_wrap->tss), 
	(*thread_proc)(data);
    } else 
    if (current_task->t_pid < 0) {
	kernel_panic(MOD_NAME "I can not create kernel thread\n");
    }
    
    return 0;
}

int threads_init(void)
{
    return 0;
}
