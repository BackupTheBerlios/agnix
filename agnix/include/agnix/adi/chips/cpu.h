#ifndef _AGNIX_CHIPS_CPU_H_
#define _AGNIX_CHIPS_CPU_H_

#include <asm/types.h>
#include <asm/irq.h>
#include <asm/desc.h>
//#include <asm/cpu_ops.h>

#define cpu_parm(chip_ptr)	((struct cpu_parm_s *)((chip_ptr)->internal_parm))

#define NR_CPUS		1

#define NCAPINTS	4	/* Currently we have 4 32-bit words worth of info */

/* Intel-defined CPU features, CPUID level 0x00000001, word 0 */
#define X86_FEATURE_FPU		(0*32+ 0) /* Onboard FPU */
#define X86_FEATURE_VME		(0*32+ 1) /* Virtual Mode Extensions */
#define X86_FEATURE_DE		(0*32+ 2) /* Debugging Extensions */
#define X86_FEATURE_PSE 	(0*32+ 3) /* Page Size Extensions */
#define X86_FEATURE_TSC		(0*32+ 4) /* Time Stamp Counter */
#define X86_FEATURE_MSR		(0*32+ 5) /* Model-Specific Registers, RDMSR, WRMSR */
#define X86_FEATURE_PAE		(0*32+ 6) /* Physical Address Extensions */
#define X86_FEATURE_MCE		(0*32+ 7) /* Machine Check Architecture */
#define X86_FEATURE_CX8		(0*32+ 8) /* CMPXCHG8 instruction */
#define X86_FEATURE_APIC	(0*32+ 9) /* Onboard APIC */
#define X86_FEATURE_SEP		(0*32+11) /* SYSENTER/SYSEXIT */
#define X86_FEATURE_MTRR	(0*32+12) /* Memory Type Range Registers */
#define X86_FEATURE_PGE		(0*32+13) /* Page Global Enable */
#define X86_FEATURE_MCA		(0*32+14) /* Machine Check Architecture */
#define X86_FEATURE_CMOV	(0*32+15) /* CMOV instruction (FCMOVCC and FCOMI too if FPU present) */
#define X86_FEATURE_PAT		(0*32+16) /* Page Attribute Table */
#define X86_FEATURE_PSE36	(0*32+17) /* 36-bit PSEs */
#define X86_FEATURE_PN		(0*32+18) /* Processor serial number */
#define X86_FEATURE_CLFLSH	(0*32+19) /* Supports the CLFLUSH instruction */
#define X86_FEATURE_DTES	(0*32+21) /* Debug Trace Store */
#define X86_FEATURE_ACPI	(0*32+22) /* ACPI via MSR */
#define X86_FEATURE_MMX		(0*32+23) /* Multimedia Extensions */
#define X86_FEATURE_FXSR	(0*32+24) /* FXSAVE and FXRSTOR instructions (fast save and restore */
				          /* of FPU context), and CR4.OSFXSR available */
#define X86_FEATURE_XMM		(0*32+25) /* Streaming SIMD Extensions */
#define X86_FEATURE_XMM2	(0*32+26) /* Streaming SIMD Extensions-2 */
#define X86_FEATURE_SELFSNOOP	(0*32+27) /* CPU self snoop */
#define X86_FEATURE_HT		(0*32+28) /* Hyper-Threading */
#define X86_FEATURE_ACC		(0*32+29) /* Automatic clock control */
#define X86_FEATURE_IA64	(0*32+30) /* IA-64 processor */



/* AMD-defined CPU features, CPUID level 0x80000001, word 1 */
/* Don't duplicate feature flags which are redundant with Intel! */
#define X86_FEATURE_SYSCALL	(2*32+11) /* SYSCALL/SYSRET */
#define X86_FEATURE_MMXEXT	(2*32+22) /* AMD MMX extensions */
#define X86_FEATURE_LM		(2*32+29) /* Long Mode (x86-64) */
#define X86_FEATURE_3DNOWEXT	(2*32+30) /* AMD 3DNow! extensions */
#define X86_FEATURE_3DNOW	(2*32+31) /* 3DNow! */

/* Transmeta-defined CPU features, CPUID level 0x80860001, word 2 */
#define X86_FEATURE_RECOVERY	(3*32+ 0) /* CPU in recovery mode */
#define X86_FEATURE_LONGRUN	(3*32+ 1) /* Longrun power control */
#define X86_FEATURE_LRTI	(3*32+ 3) /* LongRun table interface */

/* Other features, Linux-defined mapping, word 3 */
/* This range is used for feature bits which conflict or are synthesized */
#define X86_FEATURE_CXMMX	(4*32+ 0) /* Cyrix MMX extensions */
#define X86_FEATURE_K6_MTRR	(4*32+ 1) /* AMD K6 nonstandard MTRRs */
#define X86_FEATURE_CYRIX_ARR	(4*32+ 2) /* Cyrix ARRs (= MTRRs) */
#define X86_FEATURE_CENTAUR_MCR	(4*32+ 3) /* Centaur MCRs (= MTRRs) */

#define CPU_EFLAGS_CF		(1 << 0)
#define CPU_EFLAGS_PF		(1 << 2)
#define CPU_EFLAGS_AF		(1 << 4)
#define CPU_EFLAGS_ZF		(1 << 6)
#define CPU_EFLAGS_SF		(1 << 7)
#define CPU_EFLAGS_IF		(1 << 9)
#define CPU_EFLAGS_NT		(1 << 14)

#define CPU_CR0_PG		(1 << 31)
#define CPU_CR0_CD		(1 << 30)
#define CPU_CR0_NT		(1 << 29)
#define CPU_CR0_AM		(1 << 18)
#define CPU_CR0_WP		(1 << 16)
#define CPU_CR0_PG		(1 << 31)
#define CPU_CR0_NE		(1 << 5)
#define CPU_CR0_ET		(1 << 4)
#define CPU_CR0_TS		(1 << 3)
#define CPU_CR0_EM		(1 << 2)
#define CPU_CR0_MP		(1 << 1)
#define CPU_CR0_PE		(1 << 0)

#define CPU_CR4_OSXMMEXCPT	(1 << 10)
#define CPU_CR4_OSFXSR		(1 << 9)
#define CPU_CR4_PCE		(1 << 8)
#define CPU_CR4_PGE		(1 << 7)
#define CPU_CR4_MCE		(1 << 6)
#define CPU_CR4_PAE		(1 << 5)
#define CPU_CR4_PSE		(1 << 4)
#define CPU_CR4_DE		(1 << 3)
#define CPU_CR4_TSD		(1 << 2)
#define CPU_CR4_PVI		(1 << 1)
#define CPU_CR4_VME		(1 << 0)

#define cpuid_level(cpu, level) (cpu->cpu_cpuid_level >= level)

#define VENDOR_ID_LEN	16
#define MODEL_ID_LEN	64
#define MAX_CPU_MODELS	16

struct cpu_model_s {
    u8 cpu_family;
    const char *cpu_names[MAX_CPU_MODELS];
};

struct cpu_signature_s {
    u8  cpu_stepping : 4;
    u8  cpu_model    : 4;
    u8  cpu_family   : 4;
    u8  cpu_type     : 2;
    u8  dummy_1      : 2;
    u8  cpu_extended_model : 4;
    u8  cpu_extended_family;
    u8  dummy_2	  : 4;
};

struct cpu_misc_info_s {
    u8	cpu_brand_id;
    u8  cpu_chunks;
    u8  cpu_log_count;
    u8  cpu_apic_id;
};

struct cpu_cache_s {
    u32 dcl_1;
    u32 icl_1;
    u32 cl_2;
    u32 cl_3;
    
    u32 itlb_4K;
    u32 itlb_2M;
    u32 itlb_4M;
    u32 dtlb_4K;
    u32 dtlb_2M;
    u32 dtlb_4M;

    u32 trc;
};

#define cpu_sig		cpu_sig_union.cpu_signature
#define cpu_misc	cpu_misc_union.cpu_misc_info

struct cpu_parm_s {
    char cpu_vendor_id[VENDOR_ID_LEN];
    const char *cpu_description;
    const char *cpu_model_id;
    
    u32	 cpu_serial[4];    
    u32  cpu_brand_string[13];
    int  cpu_cpuid_level;

    union {
	struct cpu_signature_s cpu_signature;
	u32    cpu_signature_raw;
    } cpu_sig_union;

    union {
	struct cpu_misc_info_s cpu_misc_info;
	u32    cpu_misc_info_raw;
    } cpu_misc_union;

    struct cpu_cache_s cpu_cache;

    u32  cpu_features[NCAPINTS];
    u32  cpu_bugs;
    
    int  cpu_no;
    u32  cpu_bogomips;
};

int intel_init(int cpu_no);
void cpu_tsc_init(void);
void cpu_mtrr_init(void);
int tsc_get_speed(void);
int cpu_capable(u32 capable_bit);
void cpu_calibrate_delay(void);

#endif


