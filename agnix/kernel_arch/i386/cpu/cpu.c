/*
 * kernel_arch/i386/kernel/cpu/cpu.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	cpu.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <asm/cpu_ops.h>
#include <agnix/console.h>
#include <agnix/memory.h>
#include <asm/bitops.h>
#include <agnix/math64.h>

#define HIGH_WORD(x) ((x) >> 16)
#define LOW_WORD(x) ((x) & 0xFFFF)

#define CPU_FAMILIES	3
#define MAX_NR_CPU	8

struct chip_s 		cpu_chip;
struct chip_ops_s 	cpu_chip_ops;
struct cpu_parm_s  	cpu_intel_parm;

struct cpu_model_s cpu_models[] = {

    { 0x04, 
    { "486 DX-25/33", "486 DX-50", "486 SX", "486 DX/2", "486 SL", "486 SX/2",
      "unknown", "486 DX/2-WB", "486 DX/4", "486 DX/4-WB", "unknown", 
      "unknown",  "unknown", "unknown", "unknown", "unknown" }
    }, 

    { 0x05, 
    { "Pentium 60/66 A-step", "Pentium 60/66", "Pentium 75-200", "OverDrive PODP5V83",
      "Pentium MMX", "unknown", "unknown", "Pentium Mobile 75-200", "Pentium Mobile MMX",
      "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown" }
    }, 

    { 0x06, 
    { "Pentium Pro A-step",    "Pentium Pro", "Unknown", "Pentium II (Klamath)", "Unknown",
      "Pentium II (Deschutes)","Pentium II Mobile",      
      "Pentium III (Katmai)",  "Pentium III (Coppermine)", "Unknown", "Pentium III (Cascades)",
      "Unknown", "Unknown", "Unknown", "Unknown", "Unknown" }
    }, 
};

const char *cpu_descriptions[] = {
    "Unknown", "Intel Celeron", "Intel Pentium III", "Intel Pentium III Xeon",
    "Intel Pentium III", "Mobile Intel Pentium III", "Mobile Intel Celeron",
    "Intel Pentium IV", "Intel Pentium IV", "Intel Celeron", "Intel Xeon", 
    "Intel Xeon MP", "Unknown", "Intel Pentium 4", "Mobile Intel Celeron"
};

int cpu_capable(u32 capable_bit)
{
    struct cpu_parm_s *cpu_parm = cpu_parm((&cpu_chip));    

    return test_bit(capable_bit, cpu_parm->cpu_features);
}

void __init cpu_get_cache_latency(void)
{
    u8 buf[1024];
    u8 buf2[1024];
    u8 buf3[1024];
    u32 ptr, ptr2, ptr3;
    u32 tsc[2];
    u32 tsc_s[2];
    u32 tsc_e[2];
    
    ptr = ((u32)(buf + 0x20) & (~(0x20 - 1)));
    ptr2 = ((u32)(buf2 + 0x20) & (~(0x20 - 1)));
    ptr3 = ((u32)(buf3 + 0x20) & (~(0x20 - 1)));
        

    
    __asm__ __volatile__ (
    "prefetchnta (%%esi)\n\t"
    "prefetchnta 32(%%esi)\n\t"
    "prefetchnta 64(%%esi)\n\t"
    "prefetchnta 96(%%esi)\n\t"
    "prefetchnta 128(%%esi)\n\t"
    "prefetchnta 160(%%esi)\n\t"
    "prefetchnta 192(%%esi)\n\t"
    "prefetchnta 224(%%esi)\n\t"
    :
    :"S"(ptr)
    );

    rdtsc(tsc_s[0], tsc_s[1]);

/*    "prefetchnta 256(%%esi)\n\t"
    "prefetchnta 288(%%esi)\n\t"
    "prefetchnta 320(%%esi)\n\t"
    "prefetchnta 352(%%esi)\n\t"
    "prefetchnta 384(%%esi)\n\t"
    "prefetchnta 416(%%esi)\n\t"
    "prefetchnta 448(%%esi)\n\t"
    "prefetchnta 480(%%esi)"
*/
    
    __asm__ __volatile__ (
    "movl (%%esi), %%eax\n\t"
    "movl 32(%%esi), %%eax\n\t"
    "movl 64(%%esi), %%eax\n\t"
    "movl 96(%%esi), %%eax\n\t"
    "movl 128(%%esi), %%eax\n\t"
    "movl 160(%%esi), %%eax\n\t"
    "movl 192(%%esi), %%eax\n\t"
    "movl 224(%%esi), %%eax\n\t"

/*    "movl 256(%%esi), %%eax\n\t"
    "movl 288(%%esi), %%eax\n\t"
    "movl 320(%%esi), %%eax\n\t"
    "movl 352(%%esi), %%eax\n\t"
    "movl 384(%%esi), %%eax\n\t"
    "movl 416(%%esi), %%eax\n\t"
    "movl 448(%%esi), %%eax\n\t"
    "movl 480(%%esi), %%eax"*/
    :
    :
    );
    
    rdtsc(tsc_e[0], tsc_e[1]);
    sub_64_64(tsc, tsc_e, tsc_s);
    printk("\tL1 latency: %d cycles\n", (tsc[0] / 8));

//    rdtsc(tsc_s[0], tsc_s[1]);

    __asm__ __volatile__ (
    "prefetcht1 0(%%esi)\n\t"
    "prefetcht1 32(%%esi)\n\t"
    "prefetcht1 64(%%esi)\n\t"
    "prefetcht1 96(%%esi)\n\t"
    "prefetcht1 128(%%esi)\n\t"
    "prefetcht1 160(%%esi)\n\t"
    "prefetcht1 192(%%esi)\n\t"
    "prefetcht1 224(%%esi)\n\t"
    
/*    "prefetcht1 256(%%esi)\n\t"
    "prefetcht1 288(%%esi)\n\t"
    "prefetcht1 320(%%esi)\n\t"
    "prefetcht1 352(%%esi)\n\t"
    "prefetcht1 384(%%esi)\n\t"
    "prefetcht1 416(%%esi)\n\t"
    "prefetcht1 448(%%esi)\n\t"
    "prefetcht1 480(%%esi)"*/
    :
    :"S"(ptr2));

    rdtsc(tsc_s[0], tsc_s[1]);

    __asm__ __volatile__ (
    "movl (%%esi), %%eax\n\t"
    "movl 32(%%esi), %%eax\n\t"
    "movl 64(%%esi), %%eax\n\t"
    "movl 96(%%esi), %%eax\n\t"
    "movl 128(%%esi), %%eax\n\t"
    "movl 160(%%esi), %%eax\n\t"
    "movl 192(%%esi), %%eax\n\t"
    "movl 224(%%esi), %%eax\n\t"

/*    "movl 256(%%esi), %%eax\n\t"
    "movl 288(%%esi), %%eax\n\t"
    "movl 320(%%esi), %%eax\n\t"
    "movl 352(%%esi), %%eax\n\t"
    "movl 384(%%esi), %%eax\n\t"
    "movl 416(%%esi), %%eax\n\t"
    "movl 448(%%esi), %%eax\n\t"
    "movl 480(%%esi), %%eax"*/
    :
    :
    );

    rdtsc(tsc_e[0], tsc_e[1]);
    sub_64_64(tsc, tsc_e, tsc_s);
    printk("\tL2 latency: %d cycles\n", (tsc[0] / 8));


    rdtsc(tsc_s[0], tsc_s[1]);

    __asm__ __volatile__ (
    "movl (%%esi), %%eax\n\t"
    "movl 32(%%esi), %%eax\n\t"
    "movl 64(%%esi), %%eax\n\t"
    "movl 96(%%esi), %%eax\n\t"
    "movl 128(%%esi), %%eax\n\t"
    "movl 160(%%esi), %%eax\n\t"
    "movl 192(%%esi), %%eax\n\t"
    "movl 224(%%esi), %%eax\n\t"

/*    "movl 256(%%esi), %%eax\n\t"
    "movl 288(%%esi), %%eax\n\t"
    "movl 320(%%esi), %%eax\n\t"
    "movl 352(%%esi), %%eax\n\t"
    "movl 384(%%esi), %%eax\n\t"
    "movl 416(%%esi), %%eax\n\t"
    "movl 448(%%esi), %%eax\n\t"
    "movl 480(%%esi), %%eax"*/
    :
    :"S"(ptr3)
    );

    rdtsc(tsc_e[0], tsc_e[1]);
    sub_64_64(tsc, tsc_e, tsc_s);
    printk("\tRAM latency: %d cycles\n", (tsc[0] / 8));
}

void __init cpu_get_speed(void)
{
    int speed;

    if (!(read_cr4() & CPU_CR4_TSD)) {
	speed = tsc_get_speed();
        printk("\tcpu_speed: %d.%d MHz\n", speed / 1000, speed % 1000);
    }
}

void cpu_print_info(struct cpu_parm_s *cpu)
{
    if (cpu->cpu_misc.cpu_brand_id > 0) {
        printk("CPU_%d: %s processor\n", cpu->cpu_no, cpu->cpu_description);
	printk("\tcpu_model: %s\n", cpu->cpu_model_id);
    } else
	printk("CPU_%d: %s\n", cpu->cpu_no, cpu->cpu_model_id);

    printk("\tcpu_serial: %04x-%04x-%04x-%04x-%04x-%04x\n", 
        HIGH_WORD(cpu->cpu_serial[2]), LOW_WORD(cpu->cpu_serial[2]),
	HIGH_WORD(cpu->cpu_serial[2]), LOW_WORD(cpu->cpu_serial[1]),
        HIGH_WORD(cpu->cpu_serial[2]), LOW_WORD(cpu->cpu_serial[0]));

#ifdef DEBUG_CPU
    printk("\tcpuid_max_level   = %d\n",   cpu->cpu_cpuid_level);
    printk("\tcpu_vendor_id     = %s\n",   cpu->cpu_vendor_id);
    printk("\tcpu_apic_id       = %08x\n", cpu->cpu_misc.cpu_apic_id);
    printk("\tcpu_logical_count = %08x\n", cpu->cpu_misc.cpu_log_count);
    printk("\tcpu_clflush_size  = %08x\n", cpu->cpu_misc.cpu_chunks);
    printk("\tcpu_brand_index   = %08x\n", cpu->cpu_misc.cpu_brand_id);
    printk("\tcpu_features      = %08x%08x\n\n", cpu->cpu_features[1], cpu->cpu_features[0]);

    printk("\texfamily exmodel type family model stepping\n");
    printk("\t%08x  %04x    %02x   %04x  %04x    %04x\n", cpu->cpu_sig.cpu_extended_family, cpu->cpu_sig.cpu_extended_model,
			 				  cpu->cpu_sig.cpu_type,  cpu->cpu_sig.cpu_family,
							  cpu->cpu_sig.cpu_model, cpu->cpu_sig.cpu_stepping);
    
#endif

#ifdef DEBUG_CPU
    printk("\tcache_sizes:\n");
    printk("\t\td_cache_level_1 = %dKB\n", cpu->cpu_cache.dcl_1);
    printk("\t\ti_cache_level_1 = %dKB\n", cpu->cpu_cache.icl_1);
    printk("\t\tcache_level_2   = %dKB\n", cpu->cpu_cache.cl_2);
    printk("\t\tcache_level_3   = %dKB\n", cpu->cpu_cache.cl_3);
    printk("\t\ttrace_cache     = %duops\n", cpu->cpu_cache.trc);
#else
    printk("\tcache_sizes: ");
    printk("d_L1 %dKB, ", cpu->cpu_cache.dcl_1);
    printk("i_L1 %dKB, ", cpu->cpu_cache.icl_1);
    printk("L2 %dKB, ", cpu->cpu_cache.cl_2);
    printk("L3 %dKB, ", cpu->cpu_cache.cl_3);
    printk("TC %dKB\n", cpu->cpu_cache.trc);
#endif

#ifdef DEBUG_CPU
    printk("\t\ti_tlb_4K = %d ent  i_tlb_2M = %d ent  i_tlb_4M = %d ent\n", 
	    cpu->cpu_cache.itlb_4K, cpu->cpu_cache.itlb_2M, cpu->cpu_cache.itlb_4M);
    printk("\t\td_tlb_4K = %d ent  d_tlb_2M = %d ent  d_tlb_4M = %d ent\n", 
	    cpu->cpu_cache.dtlb_4K, cpu->cpu_cache.dtlb_2M, cpu->cpu_cache.dtlb_4M);
#endif

//    printk(NEW_LINE);
}

void __init cpu_parse_model_id(struct cpu_parm_s *cpu)
{
    int i;

    if (cpu->cpu_misc.cpu_brand_id > 0)
	cpu->cpu_description = cpu_descriptions[cpu->cpu_misc.cpu_brand_id];
    
    for (i = 0; i < CPU_FAMILIES; i++) {
	if (cpu_models[i].cpu_family == cpu->cpu_sig.cpu_family) {
	    cpu->cpu_model_id = cpu_models[i].cpu_names[cpu->cpu_sig.cpu_model];
	    break;
	}
    }
}

void __init cpu_get_cache_size(struct cpu_parm_s *cpu)
{
    u32 reg[4];
    u8  *reg_ch = (u8 *)reg;
    int cycles;
    int i, j;
    u32	dcl_1, icl_1, cl_2, cl_3;
    u32 itlb_4K, itlb_2M, itlb_4M;
    u32 dtlb_4K, dtlb_2M, dtlb_4M;
    u32 trc;
    
    dcl_1 = icl_1 = cl_2 = cl_3 = 0;
    itlb_4K = itlb_2M = itlb_4M = 0;
    dtlb_4K = dtlb_2M = dtlb_4M = 0;
    trc = 0;

    if (!cpuid_level(cpu, 0x0002)) 
	return;
	
    cycles = (int)cpuid_eax(0x0002) & 0xFF;

    for (i = 0; i < cycles; i++) {
	cpuid(0x0002, &reg[0], &reg[1], &reg[2], &reg[3]);
		
	for (j = 0; j < 16; j++) {
	    u8 regl, regh;
	    
	    if (reg_ch[j] & 0x80)
		continue;
		
	    regh = reg_ch[j] >> 4;
	    regl = reg_ch[j] & 0xF;
	    switch(regh) {
		case 0:
		    switch(regl) {
			/* in entries */
			case 0x1: itlb_4K += 32; break;
			case 0x2: itlb_4M += 2;  break;
			case 0x3: dtlb_4K += 64; break;
			case 0x4: dtlb_4M += 8;  break;
			/* in k-bytes */
			case 0x6: icl_1	+= 8;    break;
			case 0x8: icl_1	+= 16;   break;
			case 0xA: dcl_1 += 8;    break;
			case 0xC: dcl_1 += 16;   break;
		    } break;
		    
		case 2: 
			if (regl >= 2 && regl <= 9)
			    cl_3 += (regl - 1) << 9;
			    
			if (regl == 0xC)
			    dcl_1 += 32;
			    
			break;
		
		case 3:
		    switch(regl) {
			/* in k-bytes */
			case 0x0: icl_1 += 32;	 break;
			case 0x9: 
			case 0xB: cl_2  += 128;  break;
			case 0xC: cl_2  += 256;  break;
		    } break;
		
		case 4:
		    if (regl >= 1 && regl <= 5)
			cl_2 += 128 << (regl - 1);
		    break;
		
		case 5:
		    if (regl <= 2) {
			itlb_4K += 64 << regl;
			itlb_2M += 64 << regl;
			itlb_4M += 64 << regl;
		    } else
		    if (regl >= 0xB && regl <= 0xD) {
			dtlb_4K += 64 << (regl - 0xB);
			dtlb_4M += 64 << (regl - 0xB);
		    }
		    break;
	
		case 6:
		    if (regl >= 6 && regl <= 8)
			dcl_1 += 8 <<  (regl - 6);
		    break;
	
		case 7:
		    switch(regl) {
			/* in u-ops */
			case 0: trc += 12; 	 break;
			case 1: trc += 16;	 break;
			case 2: trc += 32; 	 break;

			default:
			    if (regl >= 0x9 && regl <= 0xC) {
				cl_2 += 128 << (regl - 0x9);
			    }
 		    } break;
		
		case 8:
		    if (regl >= 2 && regl <= 5)
			cl_2 += 256 << (regl - 2);
		
		    if (regl >= 6 && regl <= 7) 
			cl_2 += 512 << (regl - 6);
		    break;
		
		case 0xB:
		    switch (regl) {
			case 0: itlb_4K += 128; break;
			case 3: dtlb_4K += 128; break;
		    }
		    break;
	    }
	}
    }
    
    cpu->cpu_cache.dcl_1 = dcl_1;
    cpu->cpu_cache.icl_1 = icl_1;
    cpu->cpu_cache.cl_2 = cl_2;
    cpu->cpu_cache.cl_3 = cl_3;
    cpu->cpu_cache.itlb_4K = itlb_4K;
    cpu->cpu_cache.itlb_2M = itlb_2M;    
    cpu->cpu_cache.itlb_4M = itlb_4M;
    cpu->cpu_cache.dtlb_4K = dtlb_4K;
    cpu->cpu_cache.dtlb_2M = dtlb_2M;    
    cpu->cpu_cache.dtlb_4M = dtlb_4M;
}

void __init cpu_get_signature(struct cpu_parm_s *cpu)
{
    u32 eax, ebx, ecx, edx;

    if (!cpuid_level(cpu, 0x0001)) 
	return;
	
    cpuid(0x0001, &eax, &ebx, &ecx, &edx);
    
    cpu->cpu_sig_union.cpu_signature_raw  = eax;
    cpu->cpu_misc_union.cpu_misc_info_raw = ebx;
    cpu->cpu_features[0] = edx;
    cpu->cpu_features[1] = ecx;
    cpu->cpu_serial[2] = eax;
    
    /* AMD features */
}

void __init cpu_get_vendor_id(struct cpu_parm_s *cpu)
{
    if (!cpuid_level(cpu, 0x0000)) 
	return;
    
    cpuid(0x0000, (u32 *)&cpu->cpu_cpuid_level,           (u32 *)cpu->cpu_vendor_id,
	          (u32 *)(cpu->cpu_vendor_id + 8), (u32 *)(cpu->cpu_vendor_id + 4));
}

void __init cpu_get_brand_string(struct cpu_parm_s *cpu)
{
    int i, j;

    if (!cpuid_level(cpu, 0x0001)) 
	return;
		
    for (i = 0, j = 0; i < 3; i+=4, j++) 
    
    cpuid(0x80000002 + j, &cpu->cpu_brand_string[i],     &cpu->cpu_brand_string[i + 1],
                          &cpu->cpu_brand_string[i + 2], &cpu->cpu_brand_string[i + 3]);
			      
}

void __init cpu_get_serial_number(struct cpu_parm_s *cpu)
{
    u32 eax, ebx;

//    if (!cpuid_level(cpu, 0x0003)) 
//	return;
    
    cpuid(0x0003, &eax, &ebx, &cpu->cpu_serial[0], &cpu->cpu_serial[1]);
}

int __init cpu_init(struct chip_s *cpu_chip)
{
    struct cpu_parm_s *cpu_parm = cpu_parm(cpu_chip);    

    memset(cpu_parm, 0, sizeof(struct cpu_parm_s));

    cpu_get_vendor_id(cpu_parm);
    cpu_get_signature(cpu_parm);
    cpu_get_cache_size(cpu_parm);
    cpu_get_serial_number(cpu_parm);
    cpu_parse_model_id(cpu_parm);
    cpu_print_info(cpu_parm);

    cpu_tsc_init();
//    cpu_mtrr_init();
    cpu_get_speed();

    return 0;
}

int __init intel_init(int cpu_no)
{
    memset(&cpu_chip, 0, sizeof(cpu_chip));
    
    cpu_chip.chip_name  = "CPU";
    cpu_chip.chip_ops   = &cpu_chip_ops;
    
    cpu_chip_ops.init        = cpu_init;
    cpu_chip_ops.release     = 0;
    cpu_chip.internal_parm   = (void *)&cpu_intel_parm;
    cpu_chip.chip_class      = CHIP_CLASS_CPU;
    cpu_chip.chip_vendor     = 0x8086;
    cpu_chip.chip_priority   = 100;

    adi_register_chip(&cpu_chip);

    return 0;
}
