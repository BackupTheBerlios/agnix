/*
 * kernel_arch/i386/kernel/cpu/bogo.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	bogo.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>
#include <agnix/counters.h>
#include <agnix/timer.h>
#include <agnix/adi/adi.h>
#include <asm/cpu_ops.h>

#define MOD_NAME	"BOGO: "

extern struct chip_s cpu_chip;
extern int counter_jiffies_desc;

unsigned long loops_per_jiffy = (1<<12);

#define LPS_PREC 8

void __tsc_delay(unsigned long loops)
{
    u32 tsc_s[2];
    u32 tsc_c[2];

    rdtsc(tsc_s[0], tsc_s[1]);
    
    do {
        rdtsc(tsc_c[0], tsc_c[1]);
    } while ((tsc_c[0] - tsc_s[0]) < loops);
}

void __normal_delay(unsigned long loops)
{
    __asm__ __volatile__ (
    "jmp 1f\n\t"
    ".align 16\n\t"
    "1: jmp 2f\n\t"
    ".align 16\n\t"
    "2: decl %%eax\t\n"
    "jns 2b"
    :
    :"a"(loops)
    );
}

void __delay(unsigned long loops)
{
    if (!(read_cr4() & CPU_CR4_TSD))
	__tsc_delay(loops);
    else
	__normal_delay(loops);
}

void __init _cpu_calibrate_delay(void (*delay_routine)(unsigned long), const char *text)
{
    struct cpu_parm_s *cpu_parm = cpu_parm(&cpu_chip);
    u32 ticks, ticks_cur, loopbit;
    u32 jiffies_latch[2];
    int lps_precision = LPS_PREC;

    loops_per_jiffy = (1<<12);

    printk("CPU_0: Calibrating delay loop...");
    while (loops_per_jiffy <<= 1) {
    	/* wait for "start" of clock tick */
	counter_read(counter_jiffies_desc, jiffies_latch);
	ticks = jiffies_latch[0];

	do {
	    counter_read(counter_jiffies_desc, jiffies_latch);
	    ticks_cur = jiffies_latch[0];
	} while(ticks_cur == ticks);
			
	counter_read(counter_jiffies_desc, jiffies_latch);
	ticks = jiffies_latch[0];
	delay_routine(loops_per_jiffy);

	counter_read(counter_jiffies_desc, jiffies_latch);
	ticks = jiffies_latch[0] - ticks;
	if (ticks)
		break;
    }

    loops_per_jiffy >>= 1;
    loopbit = loops_per_jiffy;
    while ( lps_precision-- && (loopbit >>= 1) ) {
	loops_per_jiffy |= loopbit;
	
	counter_read(counter_jiffies_desc, jiffies_latch);
	ticks = jiffies_latch[0];

	do {
	    counter_read(counter_jiffies_desc, jiffies_latch);
	    ticks_cur = jiffies_latch[0];
	} while(ticks_cur == ticks);

	counter_read(counter_jiffies_desc, jiffies_latch);
	ticks = jiffies_latch[0];
	delay_routine(loops_per_jiffy);
	
	counter_read(counter_jiffies_desc, jiffies_latch);
	if (jiffies_latch[0] != ticks)	/* longer than 1 tick */
		loops_per_jiffy &= ~loopbit;
    }

    cpu_parm->cpu_bogomips = loops_per_jiffy;

    printk(" %d.%02d BogoMIPS (%s)\n", loops_per_jiffy/(500000/TIMER_HZ),
	    (loops_per_jiffy/(5000/TIMER_HZ)) % 100, text);
}

void __init cpu_calibrate_delay(void)
{
    _cpu_calibrate_delay(__tsc_delay, "fake");
    _cpu_calibrate_delay(__normal_delay, "true");
}
