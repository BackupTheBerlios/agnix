
#ifndef _AGNIX_ASM_TSS_H_
#define _AGNIX_ASM_TSS_H_

#include <asm/types.h>
#include <asm/desc.h>

#define IO_BITMAP_SIZE	32
#define __TSS(n) (n + __FIRST_TSS_ENTRY)
#define __TSS_BACK(n) (n - __FIRST_TSS_ENTRY)

#define INIT_TSS \
{				\
    ss0:  __KERNEL_DS,		\
    bitmap: 0x8000,		\
    io_bitmap: {~0,},		\
}

struct tss_regs_s {
	u32	eip;
	u32	eflags;
	u32	eax,ecx,edx,ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u16	es, __esh;
	u16	cs, __csh;
	u16	ss, __ssh;
	u16	ds, __dsh;
	u16	fs, __fsh;
	u16	gs, __gsh;
};

struct tss_s {
	u16	task_link,__blh;
	u32	esp0;
	u16	ss0,__ss0h;
	u32	esp1;
	u16	ss1,__ss1h;
	u32	esp2;
	u16	ss2,__ss2h;
	u32	__cr3;
	u32	eip;
	u32	eflags;
	u32	eax,ecx,edx,ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u16	es, __esh;
	u16	cs, __csh;
	u16	ss, __ssh;
	u16	ds, __dsh;
	u16	fs, __fsh;
	u16	gs, __gsh;
	u16	ldt, __ldth;
	u16	trace, bitmap;
	u32	io_bitmap[IO_BITMAP_SIZE+1];
	u32 	__cacheline_filler[5];
};

#endif

