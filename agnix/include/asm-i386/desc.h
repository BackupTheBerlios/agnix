#ifndef __ARCH_DESC_H
#define __ARCH_DESC_H

#define __FIRST_TSS_ENTRY 16
#define __FIRST_LDT_ENTRY (__FIRST_TSS_ENTRY+10)

#define __LDT(n) (((n)<<2) + __FIRST_LDT_ENTRY)

#ifndef __ASSEMBLY__
struct desc_struct {
	unsigned long a,b;
};

extern struct desc_struct gdt_table[];
extern struct desc_struct *idt, *gdt;

struct Xgt_desc_struct {
	unsigned short size;
	unsigned long address __attribute__((packed));
};

#define idt_descr (*(struct Xgt_desc_struct *)((char *)&idt - 2))
#define gdt_descr (*(struct Xgt_desc_struct *)((char *)&gdt - 2))

#define load_TR(n) __asm__ __volatile__("ltr %%ax"::"a" (__TSS(n)<<3))

#define __load_LDT(n) __asm__ __volatile__("lldt %%ax"::"a" (__LDT(n)<<3))

void set_code_desc(u32 entry, u32 base, u32 limit, u8 dpl, u8 db);
void set_data_desc(u32 entry, u32 base, u32 limit, u8 dpl, u8 db);
void set_IDT_gate_descriptor(struct desc_struct *, u16, void *, u8); 
void set_interrupt_gate(u32, u16, void *, u8); 
void set_trap_gate(u32, u16, void *, u8); 
void set_task_gate(u32 entry, u16 selector, u8 dpl); 
void set_TSS_desc(u32, void *, u32, u8); 
void set_tss_desc(unsigned int, void *addr); 

#endif /* !__ASSEMBLY__ */

#endif
