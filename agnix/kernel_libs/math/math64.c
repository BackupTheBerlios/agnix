/*
 * kernel_libs/math/math64.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	math64.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/chips/cpu.h>
#include <agnix/console.h>

void add_64_reg(u32 *value, u32 *op1, u32 *op2)
{
    __asm__ (
    "  addl (%1), %%ebx\t\n"
    "  adcl 4(%1), %%ecx\t\n"
    "  movl %%ebx, (%0)\t\n"
    "  movl %%ecx, 4(%0)\t\n"
    :
    :"r"(value), "r"(op2), "b"(*op1), "c"(*(op1 + 4))
    );
}

void add_64_xmm2(u32 *value, u32 *op1, u32 *op2)
{
    __asm__ __volatile__ (
    "  movq (%1), %%mm0\t\n"
    "  movq (%2), %%mm1\t\n"
    "  paddq %%mm0, %%mm1\t\n"
    "  movq %%mm0, (%0)"
    :
    :"r"(value), "r"(op1), "r"(op2)
    );
}

void add_64_64(u32 *value, u32 *op1, u32 *op2)
{
    if (cpu_capable(X86_FEATURE_XMM2))
	add_64_xmm2(value, op1, op2);
    else
	add_64_reg(value, op1, op2); 
}

void add_64_32(u32 *value, u32 *op1, u32 op2)
{
    __asm__ (
    "  addl %%eax, %%ebx\t\n"
    "  adcl $0, %%ecx\t\n"
    "  movl %%ebx, (%0)\t\n"
    "  movl %%ecx, 4(%0)\t\n"
    :
    :"r"(value), "a"(op2), "b"(*op1), "c"(*(op1 + 1))
    );
}

void sub_64_reg(u32 *value, u32 *op1, u32 *op2)
{
    __asm__ (
    "  subl (%1), %%ebx\t\n"
    "  sbbl 4(%1), %%ecx\t\n"
    "  movl %%ebx, (%0)\t\n"
    "  movl %%ecx, 4(%0)\t\n"
    :
    :"r"(value), "r"(op2), "b"(*op1), "c"(*(op1 + 1))
    );
}

void sub_64_xmm2(u32 *value, u32 *op1, u32 *op2)
{
    __asm__ __volatile__ (
    
    "  movq (%1), %%mm0\t\n"
    "  movq (%2), %%mm1\t\n"
    "  psubq %%mm0, %%mm1\t\n"
    "  movq %%mm0, (%0)"
    
    :"=r"(value)
    :"r"(op1), "r"(op2)
    );
}

void sub_64_64(u32 *value, u32 *op1, u32 *op2)
{
    if (cpu_capable(X86_FEATURE_XMM2))
	sub_64_xmm2(value, op1, op2);
    else
	sub_64_reg(value, op1, op2); 
}

void sub_64_32(u32 *value, u32 *op1, u32 op2)
{
    __asm__ (
    "  subl %%eax, %%ebx\t\n"
    "  sbbl $0, %%ecx\t\n"
    "  movl %%ebx, (%0)\t\n"
    "  movl %%ecx, 4(%0)\t\n"
    :
    :"r"(value), "a"(op2), "b"(*op1), "c"(*(op1 + 1))
    );
}

/* 64 bit divide */
void div_64_32(u32 *value, u32 *op1, u32 op2)
{
    __asm__ __volatile__ (
    
    "  divl %%ecx"
    
    :"=a"(*value)
    :"a"(op1[0]), "d"(op1[1]), "c"(op2)
    );
}

/* 32 bit mul */
void mul_32_32(u32 *value, u32 op1, u32 op2)
{
    __asm__ __volatile__ (
    "  mull %%ecx"
    :"=a"(value[0]), "=d"(value[1])
    :"a"(op1), "c"(op2)
    );
}

