/*
 * kernel_arch/i386/kernel/ioport.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	ioport.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <asm/types.h>
#include <asm/irq.h>

#define IN(s, b, r)  \
b in##s(u16 port) { \
    b val; \
    __asm__ __volatile__ ("in" #s " %%dx, %%" #r :"=a"(val) :"Nd"(port)); \
    return val; \
}

#define OUT(s, b, r) \
void out##s(b val, u16 port) \
{ \
    __asm__ __volatile__ ("out" #s " %%" #r ",%%dx" ::"a"(val), "Nd"(port)); \
}

IN(b,  u8,  al );
IN(w,  u16, ax );
IN(l,  u32, eax);
OUT(b, u8,  al );
OUT(w, u16, ax );
OUT(l, u32, eax);

