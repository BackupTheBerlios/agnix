#ifndef __AGNIX_ASM_REGS_H__
#define __AGNIX_ASM_REGS_H__

#include <asm/types.h>

#define REG_SAVE \
    "cld\n\t"		\
    "pushl %eax\n\t"	\
    "pushl %ebx\n\t"	\
    "pushl %ecx\n\t"	\
    "pushl %edx\n\t"	\
    "pushl %esi\n\t"	\
    "pushl %edi\n\t"	\
    "pushl %ebp\n\t"	\
    "pushl %ds\n\t"	\
    "pushl %es\n\t"	

#define REG_RESTORE \
    "popl %es\n\t"	\
    "popl %ds\n\t"	\
    "popl %ebp\n\t"	\
    "popl %edi\n\t"	\
    "popl %esi\n\t"	\
    "popl %edx\n\t"	\
    "popl %ecx\n\t"	\
    "popl %ebx\n\t"	\
    "popl %eax\n\t"	

struct regs_s {
    u32 es;
    u32 ds;
    u32 ebp;
    u32 edi;
    u32 esi;
    u32 edx;
    u32 ecx;
    u32 ebx;
    u32 eax;
    u32 eip;
    u32 cs;
    u32 eflags;
//    u32 esp;
//    u32 ss;
};

#endif
