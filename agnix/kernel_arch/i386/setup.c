/*
 * kernel_arch/i386/kernel/setup.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	setup.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/init.h>
#include <agnix/irq.h>
#include <agnix/faults.h>

extern int irqs_init(void);
extern int timer_init(void);
extern int bios_init(void);

int __init i386_init(void)
{
    faults_init();     
    irqs_init();     
    timer_init();
    bios_init();

    return 0;
}
