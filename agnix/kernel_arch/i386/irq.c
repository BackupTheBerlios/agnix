/*
 * kernel_arch/i386/kernel/irq.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	irq.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */
 
#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/tasks.h>
#include <agnix/linkage.h>
#include <agnix/console.h>
#include <agnix/memory.h>
#include <agnix/irq.h>
#include <asm/faults.h>
#include <agnix/list.h>
#include <agnix/sched.h>
#include <agnix/signals.h>
#include <agnix/fastrand.h>
#include <asm/segment.h>

#define __KERNEL_SYSCALLS__

asmlinkage void syscall_irq(struct regs_s regs);
asmlinkage void execute_irq(u32 irq, struct regs_s regs);

asmlinkage void IRQ_syscall(void);
    __asm__ (
    "\n.align 16,0x90\n"
    "IRQ_syscall:\n\t"
    REG_SAVE
    "call syscall_irq\n\t"
    REG_RESTORE
    "iret\n\t"
    );

#define IRQ_BUILD(nr,offset) 		\
asmlinkage void IRQ_##nr##offset(void);	\
    __asm__ (	\
    "\n.align 16,0x90\n"	\
    "IRQ_"#nr#offset":\n\t"	\
    REG_SAVE			\
    "pushl $0x"#nr#offset"\n\t"	\
    "call execute_irq\n\t"	\
    "addl $4, %esp\n\t"		\
    REG_RESTORE			\
    "iret\n\t"			\
    );

#define IRQ_BUILD_NAME(nr, offset)	\
    irq_names[nr * 16 + 0x##offset] = IRQ_##nr##offset;

#define IRQ_BUILD_16(x) \
    IRQ_BUILD(x,0); IRQ_BUILD(x,1); IRQ_BUILD(x,2); IRQ_BUILD(x,3);	\
    IRQ_BUILD(x,4); IRQ_BUILD(x,5); IRQ_BUILD(x,6); IRQ_BUILD(x,7);	\
    IRQ_BUILD(x,8); IRQ_BUILD(x,9); IRQ_BUILD(x,A); IRQ_BUILD(x,B);	\
    IRQ_BUILD(x,C); IRQ_BUILD(x,D); IRQ_BUILD(x,E); IRQ_BUILD(x,F);

#define IRQ_BUILD_NAME_16(x) \
    IRQ_BUILD_NAME(x,0); IRQ_BUILD_NAME(x,1); IRQ_BUILD_NAME(x,2); IRQ_BUILD_NAME(x,3);	\
    IRQ_BUILD_NAME(x,4); IRQ_BUILD_NAME(x,5); IRQ_BUILD_NAME(x,6); IRQ_BUILD_NAME(x,7);	\
    IRQ_BUILD_NAME(x,8); IRQ_BUILD_NAME(x,9); IRQ_BUILD_NAME(x,A); IRQ_BUILD_NAME(x,B);	\
    IRQ_BUILD_NAME(x,C); IRQ_BUILD_NAME(x,D); IRQ_BUILD_NAME(x,E); IRQ_BUILD_NAME(x,F);

IRQ_BUILD_16(0);

extern struct chip_s i8259a_chip;
extern u32 random_val;

struct desc_struct idt_table[256] __attribute__((__section__("data.idt"))) = { {0, 0}, };
struct irq_s irq_info[256];
void (*irq_names[256])(void);

asmlinkage void execute_irq(u32 irq, struct regs_s regs)
{
    struct irq_s *irq_struct = &irq_info[irq];
    struct irq_routine_s *irq_proc;
    struct chip_s *chip = &i8259a_chip;
    struct list_head *tmp;
    void (*proc)(u32);
    u32 data;

    if (!list_empty(&irq_struct->head)) {
	list_for_each(tmp, &irq_struct->head) {
	    irq_proc = list_entry(tmp, struct irq_routine_s, list);
	    proc = irq_proc->proc;
	    data = irq_proc->data;
	
	    if (proc) {
		proc(data);
	    }

	    if (irq_proc->flags & IRQ_FLAG_RANDOM)
	        random_val = _fastrand(random_val);
	}
    }
    
    pic_ops(chip)->ack_irq(chip, irq);

    if (current_task->signals.pending)
	execute_signals(current_task, &regs);

    if (current_task->t_state &= TASK_STAT_RESCHED)
	schedule_task();
}

int install_irq(u8 irq, struct irq_routine_s *irq_proc)
{
    struct chip_s *chip = &i8259a_chip;
    struct irq_s *irq_struct = &irq_info[irq];

    if (list_empty(&irq_struct->head)) 
	list_add(&irq_proc->list, &irq_struct->head);
    else {
	list_add(&irq_proc->list, &irq_struct->head);
    }

    pic_ops(chip)->enable_irq(chip, irq);
    
    return 0;
}

int __init irqs_init(void)
{
    int i;
    u32	flags;
    save_flags(flags); __cli();

    memset(irq_info, 0, 256 * sizeof(struct irq_s));

    for (i = 0; i < 256; i++) {
	INIT_LIST_HEAD(&irq_info[i].head);
    }

    IRQ_BUILD_NAME_16(0);
    for (i = 0; i < 16; i++)
	set_interrupt_gate(32 + i, __KERNEL_CS, irq_names[i], 0);

    set_interrupt_gate(0x80, __KERNEL_CS, IRQ_syscall, 3);

    __asm__ __volatile__ ("lgdt %0" : "=m" (gdt_descr));
    __asm__ __volatile__ ("lidt %0" : "=m" (idt_descr));
    __asm__ __volatile__ ("pushfl ; andl $0xffffbfff, (%esp); popfl");

    restore_flags(flags);

    return 0;
}

