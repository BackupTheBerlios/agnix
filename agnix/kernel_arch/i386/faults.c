/*
 * kernel_arch/i386/kernel/faults.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	faults.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */
 
#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/adi/adi.h>
#include <agnix/linkage.h>
#include <agnix/console.h>
#include <agnix/irq.h>
#include <agnix/faults.h>
#include <asm/regs.h>
#include <asm/segment.h>

#define TRAP_BUILD(nr, routine) \
asmlinkage void TRAP_##nr(void);\
    __asm__ (	\
    "\n.align 16,0x90\n"	\
    "TRAP_"#nr":\n\t"		\
    "pushl $0\n\t"		\
    REG_SAVE			\
    "call "#routine"\n\t"	\
    REG_RESTORE			\
    "addl $4, %esp\n\t"		\
    "iret\n\t"			\
    );

#define TRAP_BUILD_ERROR(nr, routine) \
asmlinkage void TRAP_##nr(void);\
    __asm__ (	\
    "\n.align 16,0x90\n"	\
    "TRAP_"#nr":\n\t"		\
    REG_SAVE			\
    "call "#routine"\n\t"	\
    REG_RESTORE			\
    "addl $4, %esp\n\t"		\
    "iret\n\t"			\
    );

#define TRAP_BUILD_NAME(name, text)	\
asmlinkage void TRAP_##name(u32 error)	\
{					\
    printk(#text);			\
}


asmlinkage void TRAP_dev_not_avail(u32 error)
{
//    printk("device not available\n");
    __asm__ __volatile__ ("clts");
}

asmlinkage void TRAP_general_protection(u32 error)
{
//    printk("general protection %x\n", error);
}

asmlinkage void TRAP_page_fault(u32 error)
{
    u32 cr2;
    
    __asm__ __volatile__ ("movl %%cr2, %%eax"
			    :"=a"(cr2)
			    :);

//    printk("page fault %08x %x\n", cr2, error & 0x03);
//    printk("cr2 == %x\n", cr2);
}

TRAP_BUILD_NAME(divide_fault, "divide fault");
TRAP_BUILD_NAME(debug, "debug");
TRAP_BUILD_NAME(nmi, "nmi");
TRAP_BUILD_NAME(int_3, "int 3");
TRAP_BUILD_NAME(overflow, "overflow");
TRAP_BUILD_NAME(bounds, "bounds");
TRAP_BUILD_NAME(invalid_op, "invalid op");
TRAP_BUILD_NAME(double_fault, "double fault");
TRAP_BUILD_NAME(coprocessor_seg_overrun, "coprocessor segment overrun");
TRAP_BUILD_NAME(invalid_TSS, "invalid TSS");
TRAP_BUILD_NAME(seg_not_present, "segment not present");
TRAP_BUILD_NAME(stack_seg, "stack segment");
TRAP_BUILD_NAME(coprocessor_error, "coprocessor error");
TRAP_BUILD_NAME(align_check, "alignment check");
TRAP_BUILD_NAME(reserved, "reserved");

TRAP_BUILD(00, TRAP_divide_fault);
TRAP_BUILD(01, TRAP_debug);
TRAP_BUILD_ERROR(02, TRAP_nmi);
TRAP_BUILD(03, TRAP_int_3);
TRAP_BUILD(04, TRAP_overflow);
TRAP_BUILD(05, TRAP_bounds);
TRAP_BUILD(06, TRAP_invalid_op);
TRAP_BUILD(07, TRAP_dev_not_avail);
TRAP_BUILD_ERROR(08, TRAP_double_fault);
TRAP_BUILD(09, TRAP_coprocessor_seg_overrun);
TRAP_BUILD_ERROR(10, TRAP_invalid_TSS);
TRAP_BUILD_ERROR(11, TRAP_seg_not_present);
TRAP_BUILD_ERROR(12, TRAP_stack_seg);
TRAP_BUILD_ERROR(13, TRAP_general_protection);
TRAP_BUILD_ERROR(14, TRAP_page_fault);
TRAP_BUILD(15, TRAP_reserved);
TRAP_BUILD(16, TRAP_coprocessor_error);
TRAP_BUILD_ERROR(17, TRAP_align_check);

int faults_init(void)
{
    u32	flags;
    save_flags(flags); __cli();

    set_trap_gate(0, __KERNEL_CS, TRAP_00, 0);
    set_trap_gate(1, __KERNEL_CS, TRAP_01, 0);
    set_interrupt_gate(2, __KERNEL_CS, TRAP_02, 0);
    set_interrupt_gate(3, __KERNEL_CS, TRAP_03, 0);
    set_interrupt_gate(4, __KERNEL_CS, TRAP_04, 0);
    set_interrupt_gate(5, __KERNEL_CS, TRAP_05, 0);
    set_trap_gate(6, __KERNEL_CS, TRAP_06, 0);
    set_trap_gate(7, __KERNEL_CS, TRAP_07, 0);
    set_trap_gate(8, __KERNEL_CS, TRAP_08, 0);
    set_trap_gate(9, __KERNEL_CS, TRAP_09, 0);
    set_trap_gate(10, __KERNEL_CS, TRAP_10, 0);
    set_trap_gate(11, __KERNEL_CS, TRAP_11, 0);
    set_trap_gate(12, __KERNEL_CS, TRAP_12, 0);
    set_trap_gate(13, __KERNEL_CS, TRAP_13, 0);
    set_interrupt_gate(14, __KERNEL_CS, TRAP_14, 0);
    set_trap_gate(15, __KERNEL_CS, TRAP_15, 0);
    set_trap_gate(16, __KERNEL_CS, TRAP_16, 0);
    set_trap_gate(17, __KERNEL_CS, TRAP_17, 0);
    
    restore_flags(flags);    

    return 0;
}
