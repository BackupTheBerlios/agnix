/*
 * kernel_arch/i386/kernel/init_task.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	init_task.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/tasks.h>
#include <agnix/memory.h>
#include <agnix/linkage.h>
#include <asm/segment.h>
#include <asm/desc.h>
#include <agnix/adi/chips/cpu.h>
#include <agnix/console.h>

extern asmlinkage void system_call_task(void);

u32 idle_esp[0x1000] __attribute__((__section__(".data.init_task")));
struct tss_wrap_s init_tss[MAX_TASKS] __attribute__ ((__aligned__(32)));
struct task_s *init_task[MAX_TASKS];

struct tss_s  *idle_tss;
struct task_s *idle_task;
struct tss_s  *syscall_tss;
struct task_s *syscall_task;

u16 kernel_task_init(struct task_s **task, struct tss_s **tss, u32 page, u32 eip)
{
    struct tss_wrap_s *tss_wrap;
    
    tss_wrap = get_free_tss();
    *tss = &(tss_wrap->tss);

    if (!page)
        page = get_free_pages(1);

    *task = (struct task_s *)page;
    (*task)->t_pid = 0;
    (*task)->tss_wrap = tss_wrap;
    
    return task_kernel_create(tss_wrap, eip, page + 0x1000);
}

u16 __init idle_task_init(void)
{
    u16 entry;
    
    entry = kernel_task_init(&idle_task, &idle_tss, (u32)idle_esp, 0);
    load_TR(entry);

    return entry;
}

/*u16 __init syscall_task_init(void)
{
    u16 entry;

    entry = kernel_task_init(&syscall_task, &syscall_tss, 0, (u32)system_call_task);
    syscall_tss->eflags &= ~CPU_EFLAGS_IF;
    set_task_gate(0x80, __TSS(entry) << 3, 0); 
    
    return entry;
}
*/
