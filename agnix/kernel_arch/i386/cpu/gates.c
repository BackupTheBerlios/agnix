/*
 * kernel_arch/i386/kernel/cpu/gates.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	gates.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <asm/types.h>
#include <asm/tss.h>
#include <asm/desc.h>

#define CODE_RE_DESC_TYPE	0x1A
#define DATA_RW_DESC_TYPE	0x12
#define INTERRUPT_GATE_TYPE	0x0E
#define TRAP_GATE_TYPE	 	0x0F
#define TASK_GATE_TYPE		0x05
#define TSS_DESC_TYPE		0x09
#define GATE_PRESENT_FLAG	1 << 7
#define GRANULARITY_FLAG	1 << 7
#define DB_FLAG			1 << 6

#define math_type_byte(type) \
    u8 type_byte = 0;			\
    type_byte |= GATE_PRESENT_FLAG;	\
    type_byte |= type;			\
    type_byte |= dpl << 5;		

extern struct desc_struct idt_table[256];

void set_IDT_gate_descriptor(struct desc_struct *desc_table, u16 selector, void *offset, u8 type_byte)
{
    u16 *table = (u16 *)desc_table;
    u32 offset_32 = (u32)offset;
    
    table[0] = (u16)(offset_32 & 0xFFFF);
    table[1] = selector;
    table[2] = (type_byte << 8);
    table[3] = (u16)((offset_32 >> 16) & 0xFFFF);
}

void set_TSS_descriptor(struct desc_struct *desc_table, void *offset, u32 limit, u8 type_byte)
{
    u16 *table = (u16 *)desc_table;
    u32 base_32 = (u32)offset;
    
    table[0] = (u16)(limit   & 0xFFFF);
    table[1] = (u16)(base_32 & 0xFFFF);
    table[2] = (u16)((type_byte << 8) | ((base_32 >> 16) & 0xFF));
    table[3] = (u16)(((base_32 >> 16) & 0xFF00) | ((limit >> 16) & 0x0F));
}

void set_GDT_descriptor(struct desc_struct *desc_table, u32 base, u32 limit, u8 type_byte, u8 db)
{
    u16 *table = (u16 *)desc_table;
    
    table[0] = (u16)(limit & 0xFFFF);
    table[1] = (u16)(base & 0xFFFF);
    table[2] = (u16)((type_byte << 8) | ((base >> 16) & 0xFF));
    table[3] = (u16)(((base >> 16) & 0xFF00) | ((limit >> 16) & 0x0F) | ((db & 0x01) << 6));
}

void set_code_desc(u32 entry, u32 base, u32 limit, u8 dpl, u8 db) 
{
    math_type_byte(CODE_RE_DESC_TYPE);    
    set_GDT_descriptor(gdt_table + entry, base, limit, type_byte, db);
}

void set_data_desc(u32 entry, u32 base, u32 limit, u8 dpl, u8 db) 
{
    math_type_byte(DATA_RW_DESC_TYPE);    
    set_GDT_descriptor(gdt_table + entry, base, limit, type_byte, db);
}

void set_interrupt_gate(u32 entry, u16 selector, void *offset, u8 dpl) 
{
    math_type_byte(INTERRUPT_GATE_TYPE);    
    set_IDT_gate_descriptor(idt_table + entry, selector, offset, type_byte);
}

void set_trap_gate(u32 entry, u16 selector, void *offset, u8 dpl) 
{
    math_type_byte(TRAP_GATE_TYPE);    
    set_IDT_gate_descriptor(idt_table + entry, selector, offset, type_byte);
}

void set_task_gate(u32 entry, u16 selector, u8 dpl) 
{
    math_type_byte(TASK_GATE_TYPE);    
    set_IDT_gate_descriptor(idt_table + entry, selector, 0, type_byte);
}

void set_TSS_desc(u32 entry, void *offset, u32 limit, u8 dpl) 
{
    math_type_byte(TSS_DESC_TYPE);        
    set_TSS_descriptor(gdt_table + entry, offset, limit, type_byte);
}
