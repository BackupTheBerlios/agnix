/*
 * kernel_arch/i386/kernel/task.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	task.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/init.h>
#include <asm/segment.h>
#include <agnix/adi/chips/cpu.h>	/* EFLAGS DEFS */
#include <asm/desc.h>			/* set_TSS_desc */
#include <agnix/memory.h>		/* get_free_pages, memset, memcpy */
#include <agnix/console.h>
#include <agnix/tasks.h>

extern int idle_task_init(void);
extern struct tss_wrap_s init_tss[MAX_TASKS];
extern struct main_memory_s main_memory;

struct list_head tss_list;

#define jmp_to_task(indirect) \
    __asm__ __volatile__ (	\
    "ljmp *(%%edi)\n\t"		\
    :				\
    :"D"(&indirect)		\
    )

struct indirect_s {
    u32 offset;
    u16 segment;
} task_indirect = { 0, 0 };

struct tss_wrap_s * get_free_tss(void)
{
    struct tss_wrap_s *tss = 0;

    if (!list_empty(&tss_list)) {
	if ((tss = list_entry(tss_list.next, struct tss_wrap_s, tss_list)) != 0) {
	    list_del(tss_list.next);
	    return tss;
	}
    }
    
    return 0;	
}

void put_free_tss(struct tss_wrap_s *tss)
{
    list_add_tail(&tss->tss_list, &tss_list);
}

void task_switch(u16 nr_task)
{
#ifdef DEBUG_THREAD
    printk("jmp to %d\n", nr_task);
    printk("eip == %x\n", init_tss[nr_task].tss.eip);
    printk("cr3 == %x\n", init_tss[nr_task].tss.__cr3);
    printk("cs  == %x\n", init_tss[nr_task].tss.cs);
    printk("ds  == %x\n", init_tss[nr_task].tss.ds);
    printk("esp  == %x\n", init_tss[nr_task].tss.esp);
    printk("ebp  == %x\n", init_tss[nr_task].tss.ebp);
    printk("link  == %x\n", init_tss[nr_task].tss.task_link);
    printk("trace == %x\n", init_tss[nr_task].tss.trace);
#endif

    task_indirect.segment = __TSS(nr_task) << 3;
    task_indirect.offset = 0;

    jmp_to_task(task_indirect);
}

void task_set_handler(struct task_s *task, u32 eip, u32 esp) {
    struct tss_s *tss = &(task->tss_wrap->tss);
    
    tss->eip = eip;
    tss->esp = esp;
}

void task_restore_state(struct task_s *task, struct task_state_s *state) {
/*    struct tss_s *tss_state = &(state->tss_regs);
    struct tss_s *tss_task = &(task->tss_wrap->tss);
    
    memcpy(tss_task, tss_state, sizeof(struct tss_s));*/
}

void task_save_state(struct task_s *task, struct task_state_s *state) {
/*    struct tss_s *tss_state = &(state->tss);
    struct tss_s *tss_task = &(task->tss_wrap->tss);
    
    memcpy(tss_state, tss_task, sizeof(struct tss_s));*/
}

int task_set_user_mode(struct tss_s *tss, u32 eip, u32 esp) 
{	
    /* user mode esp, cr3 */

    tss->eip  = eip;
    tss->esp  = esp;
    tss->ss0  = __KERNEL_DS;
    tss->ss1  = __KERNEL_DS;
    tss->ss2  = __KERNEL_DS;
    tss->cs   = __USER_CS;
    tss->ss   = __USER_DS;
    tss->ds   = __USER_DS;    
    tss->es   = __USER_DS;
    tss->fs   = __USER_DS;
    tss->gs   = __USER_DS;

    return 0;    
}

u16 task_kernel_create(struct tss_wrap_s *tss_wrap, u32 eip, u32 esp)
{
    struct tss_s *tss = &(tss_wrap->tss);
    u16 desc_nr;
    u16 entry;

    entry = tss_wrap->tss_entry;
    desc_nr = __TSS(entry);

    memset(tss, 0, sizeof(struct tss_s));

    tss->eflags = CPU_EFLAGS_IF;
    tss->eip  = eip;
    tss->esp  = esp;
    tss->esp0 = esp;
    tss->esp1 = 0;
    tss->esp2 = 0;
    tss->ss0  = __KERNEL_DS;
    tss->ss1  = __KERNEL_DS;
    tss->ss2  = __KERNEL_DS;
    tss->cs   = __KERNEL_CS;
    tss->ss   = __KERNEL_DS;
    tss->ds   = __KERNEL_DS;    
    tss->es   = __KERNEL_DS;
    tss->fs   = __KERNEL_DS;
    tss->gs   = __KERNEL_DS;
    tss->__cr3  = (u32)main_memory.pdbr - 0xC0000000;
    tss->bitmap = 0x8000;
    tss->io_bitmap[0] = ~0;

    set_TSS_desc(desc_nr, (void *)&init_tss[entry].tss, 235, 0);

    return entry;
}

struct tss_wrap_s * task_fork(struct regs_s *regs, u32 esp)
{
    struct tss_wrap_s *tss_wrap;
    struct tss_s *tss;
    u16 desc_nr;
    u16 entry;

    if ((tss_wrap = get_free_tss()) == 0)
	return 0;

    tss = &tss_wrap->tss;

    entry = tss_wrap->tss_entry;
    desc_nr = __TSS(entry);

    memset(tss, 0, sizeof(struct tss_s));

    tss->eax  = regs->eax;
    tss->ebx  = regs->ebx;
    tss->ecx  = regs->ecx;
    tss->edx  = regs->edx;
    tss->esp  = esp;
    tss->esp0 = esp;
    tss->ss0  = __KERNEL_DS;
    tss->ss1  = __KERNEL_DS;
    tss->ss2  = __KERNEL_DS;
    tss->esi  = regs->esi;
    tss->edi  = regs->edi;
    tss->eip  = regs->eip;
    tss->ebp  = regs->ebp;
    tss->cs   = __KERNEL_CS;
    tss->ss   = __KERNEL_DS;
    tss->ds   = __KERNEL_DS;
    tss->es   = __KERNEL_DS;
    tss->fs   = __KERNEL_DS;
    tss->gs   = __KERNEL_DS;
    tss->__cr3  = (u32)main_memory.pdbr - 0xC0000000;
    tss->eflags = regs->eflags;
    tss->task_link = 0;
    tss->bitmap = 0x8000;
    tss->io_bitmap[0] = ~0;

    set_TSS_desc(desc_nr, (void *)tss, 235, 0);

    return tss_wrap;
}

int task_exit(struct tss_wrap_s *tss_wrap)
{
    u16 desc_nr;
    u16 entry;

    put_free_tss(tss_wrap);

    entry = tss_wrap->tss_entry;
    desc_nr = __TSS(entry);

    set_TSS_desc(desc_nr, (void *)0, 235, 0);
    
    return 0;
}

int __init tasks_init(void)
{
    int i;

    INIT_LIST_HEAD(&tss_list);
    
    for (i = 0; i < MAX_TASKS; i++) {
	INIT_LIST_HEAD(&init_tss[i].tss_list);
	init_tss[i].tss_entry = (u16)i;
	put_free_tss(&init_tss[i]);
    }

    idle_task_init();

    return 0;
}

#undef DEBUG_THREAD
