#ifndef _AGNIX_ASM_CPU_OPS_H_
#define _AGNIX_ASM_CPU_OPS_H_

#include <asm/types.h>
#include <asm/atomic.h>
#include <asm/bitops.h>

void cpuid(u32 level, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx);
u32  cpuid_reg(u32 level, int reg_nr);

#define cpuid_eax(level) cpuid_reg(level, 0)
#define cpuid_ebx(level) cpuid_reg(level, 1)
#define cpuid_ecx(level) cpuid_reg(level, 2)
#define cpuid_edx(level) cpuid_reg(level, 3)


#define MSR_REG_TSC	0x10

#define rdmsr(reg, val1, val2) 			    \
    __asm__ __volatile__ ("wrmsr" 		    \
			   : "=a"(val1), "=d"(val2) \
			   : "c"(reg)		    \
			 );		

#define wrmsr(reg, val1, val2) 			    \
    __asm__ __volatile__ ("wrmsr" 		    \
    			   : 			    \
			   :"a"(val1), "d"(val2), "c"(reg) \
			 );

#define rdtsc(low, high)  			    \
    __asm__ __volatile__ ("rdtsc" 		    \
			   :"=a"(low), "=d"(high)   \
			 )			  

#define wrtsc(low, high) wrmsr(MSR_REG_TSC, low, high)

#define read_cr0() ({ \
	unsigned int __dummy; \
	__asm__( \
		"movl %%cr0,%0\n\t" \
		:"=r" (__dummy)); \
	__dummy; \
})

#define write_cr0(x) \
	__asm__("movl %0,%%cr0": :"r" (x));

#define read_cr4() ({ \
	unsigned int __dummy; \
	__asm__( \
		"movl %%cr4,%0\n\t" \
		:"=r" (__dummy)); \
	__dummy; \
})

#define write_cr4(x) \
	__asm__("movl %0,%%cr4": :"r" (x));

#define stts() write_cr0(8 | read_cr0())
#define clts() __asm__ __volatile__ ("clts")

#define wbinvd() \
	__asm__ __volatile__ ("wbinvd": : :"memory");

#endif

