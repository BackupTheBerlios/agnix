/*
 * kernel_arch/i386/kernel/machine.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	machine.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h> 
#include <agnix/bios/apm.h>
#include <agnix/console.h>

#define MOD_NAME 	"MACHINE: "

/*
 * address = 0x00000000
 * limit   = 0
 */
u32 dummy_idt[2];

void do_machine_real_mode_reboot(void)
{
    /* will be implemented */
}

/*
 * triple fault:
 * 1. int 0
 * 2. double fault
 * 3. tripple fault and reset
 */
void do_machine_hard_reboot(void)
{
    printk(MOD_NAME "Machine hard reboot... ");

    __asm__ __volatile__ ("lidt %0\n\t"
			  "int $0"
			  :
			  :"m"(dummy_idt));
			  
    printk("failed\n");
}

void do_machine_reboot(void)
{
    printk(MOD_NAME "Machine rebooting...\n");
	
    do_machine_hard_reboot();
    do_machine_real_mode_reboot();
    
    printk(MOD_NAME "Machine reboot failed\n");
    
    for(;;) {
	__asm__ __volatile__ ("hlt");
    }    
}

void do_machine_powerdown(void)
{
    printk(MOD_NAME "Machine poweroff\n");

#ifdef CONFIG_BIOS_APM
    apm_set_power_state(APM_ALL_DEVICES, APM_POWER_STATE_OFF);

    printk(MOD_NAME "Machine power off failed\n");
#endif
	
    for(;;) {
	__asm__ __volatile__ ("hlt");
    }    
}

void do_machine_halt(void)
{
    printk(MOD_NAME "Machine halt\n");

    for(;;) {
	__asm__ __volatile__ ("hlt");
    }    
}
