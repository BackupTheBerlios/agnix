/*
 * kernel_arch/i386/kernel/bios/apm.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	apm.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/bios/bios.h>
#include <asm/bios_parametrs.h>
#include <agnix/console.h>
#include <asm/desc.h>
#include <asm/segment.h>
#include <asm/paging.h>
#include <asm/irq.h>
#include <agnix/tasks.h>
#include <agnix/threads.h>
#include <agnix/unistd.h>
#include <agnix/irq.h>
#include <agnix/syscalls.h>	/* do_sys_sigaction */

#define MOD_NAME	"APM: "

const char *apm_baterry_status[] = { "high", "low", "critical", "charging" };

/* this is redefined in kernel_arch/i386/kernel/task.c */
static struct indirect_s {
    u32 offset;
    u16 segment;
} apm_indirect;

struct apm_info_s *apm_bios_info;
struct apm_power_status_s apm_power_status;

u32 eax, ebx, ecx, edx, esi, edi;

u8 apm_function_call_simple(u32 func_nr, u32 ebx_i, u32 ecx_i)
{
    u32 eax_o;
    u32 flags;
    
    save_flags(flags); __cli();

    __asm__ __volatile__ (
        "pushl %%ds\n\t"
	"pushl %%es\n\t"
        "pushl %%fs\n\t"
	"pushl %%gs\n\t"
    
        "xorl %%edx, %%edx\n\t"
	"movw %%dx, %%ds\n\t"
        "movw %%dx, %%es\n\t"
	"movw %%dx, %%fs\n\t"
        "movw %%dx, %%gs"
    :
    :
    );

    __asm__ __volatile__ (
        "pushl %%edi\n\t"
	"pushl %%ebp\n\t"
        "lcall %%cs:apm_indirect\n\t"
        "setc %%al\n\t"
	"popl %%ebp\n\t"
        "popl %%edi\n\t"

	"popl %%gs\n\t"
        "popl %%fs\n\t"
        "popl %%es\n\t"
        "popl %%ds"

    :"=a"(eax_o)
    :"D"(&apm_indirect), "a"(func_nr), "b"(ebx_i), "c"(ecx_i)
    );

    restore_flags(flags);
    
    return (u8)((eax_o) & 0xFF);
}

u8 apm_function_call_full(u32 func_nr, u32 ebx_i, u32 ecx_i,
		          u32 *eax_o, u32 *ebx_o, u32 *ecx_o, u32 *edx_o, u32 *esi_o, u32 *edi_o)
{
    u32 flags;
    
    save_flags(flags); __cli();

    __asm__ __volatile__ (
        "pushl %%ds\n\t"
	"pushl %%es\n\t"
        "pushl %%fs\n\t"
	"pushl %%gs\n\t"
    
        "xorl %%edx, %%edx\n\t"
	"movw %%dx, %%ds\n\t"
        "movw %%dx, %%es\n\t"
	"movw %%dx, %%fs\n\t"
        "movw %%dx, %%gs"
    :
    :
    );

    __asm__ __volatile__ (
        "pushl %%edi\n\t"
	"pushl %%ebp\n\t"
        "lcall %%cs:apm_indirect\n\t"
        "setc %%al\n\t"
	"popl %%ebp\n\t"
        "popl %%edi\n\t"

	"popl %%gs\n\t"
        "popl %%fs\n\t"
        "popl %%es\n\t"
        "popl %%ds"

    :"=a"(*eax_o), "=b"(*ebx_o), "=c"(*ecx_o), "=d"(*edx_o), "=S"(*esi_o), "=D"(*edi_o)
    :"D"(&apm_indirect), "a"(func_nr), "b"(ebx_i), "c"(ecx_i)
    );

    restore_flags(flags);
    
    return (u8)((*eax_o) & 0xFF);
}

int apm_cpu_idle(void)
{
    if (apm_function_call_simple(APM_CPU_IDLE, APM_DEFAULT, APM_DEFAULT))
	return -1;

    return 0;
}

int apm_cpu_busy(void)
{
    if (apm_function_call_simple(APM_CPU_BUSY, APM_DEFAULT, APM_DEFAULT))
	return -1;

    return 0;
}

int apm_set_power_state(u8 dest, u8 state)
{
    if (apm_function_call_simple(APM_SET_POWER_STATE, dest, state))
	return -1;
    
    return 0;
}

void apm_show_power_status(struct apm_power_status_s *power_status)
{
    if ((power_status->apm_battery_status != 0xFF) && (power_status->apm_battery_status <= 3))
        printk(MOD_NAME "battery status = %s\n", apm_baterry_status[power_status->apm_battery_status]);
    
    if ((power_status->apm_battery_life_perc) <= 0x64)
        printk(MOD_NAME "battery remaining = %d%%\n", power_status->apm_battery_life_perc);
}

int apm_get_power_status(void)
{
    if (apm_function_call_full(APM_GET_POWER_STATUS, APM_ALL_DEVICES, APM_DEFAULT, &eax, &ebx, &ecx, &edx, &esi, &edi))
	return -1;
    
    apm_power_status.apm_battery_status = (u8)(ebx & 0xFF);
    apm_power_status.apm_battery_flag = (u8)((ecx >> 8) & 0xFF);
    apm_power_status.apm_battery_life_perc = (u8)(ecx & 0xFF);
    apm_power_status.apm_battery_life_time = (u16)((edx >> 16) & 0xFFFF);
    
    return 0;
}

int apm_enable(void)
{
    if (apm_function_call_simple(APM_ENABLE_DEV_APM, APM_ALL_DEVICES, APM_ENABLE))
	printk(MOD_NAME "Cannot enable APM\n");

    if (apm_function_call_simple(APM_ENGAGE_APM, APM_ALL_DEVICES, APM_ENABLE))
	printk(MOD_NAME "Cannot engage APM\n");
    
    return 0;
}

void apm_signal_handler(int signal)
{
    printk("APM_SIGNAL_HANDLER %d\n", signal);
}

void apm_task_start(void *data)
{
    struct sigaction_s act;
    int i;

    __sti();
    
    act.sa_handler = apm_signal_handler;
    do_sys_sigaction(10, &act, NULL);
        
    for(;;) {
	for (i = 0; i < 700000000; i++);
    }
}    

int __init apm_init(void)
{
    apm_bios_info = APM_INFO;

    if (apm_bios_info->apm_version == 0x00) {
	printk(MOD_NAME "not found\n");
	return -1;
    }

    printk(MOD_NAME "Found APM ver. %d.%d\n", 
	(apm_bios_info->apm_version >> 8) & 0xFF, apm_bios_info->apm_version & 0xFF);

    set_data_desc(__APM_40 >> 3,    phys_to_virt((u32)(0x40 << 4)), 4095 - (0x40 << 4), 0, 1);
    set_code_desc(__APM_CS >> 3,    phys_to_virt((u32)(apm_bios_info->apm_cseg << 4)), 64 * 1024 - 1, 0, 1);
    set_code_desc(__APM_CS_16 >> 3, phys_to_virt((u32)(apm_bios_info->apm_cseg_16 << 4)), 64 * 1024 - 1, 0, 0);
    set_data_desc(__APM_DS >> 3,    phys_to_virt((u32)(apm_bios_info->apm_dseg << 4)), 64 * 1024 - 1, 0, 1);

    /*
      set_code_desc(__APM_CS >> 3,    phys_to_virt((u32)(apm_bios_info->apm_cseg << 4)), (apm_bios_info->apm_cseg_len - 1) & 0xffff, 0, 1);
      set_code_desc(__APM_CS_16 >> 3, phys_to_virt((u32)(apm_bios_info->apm_cseg_16 << 4)), (apm_bios_info->apm_cseg_16_len - 1) & 0xffff, 0, 0);
      set_data_desc(__APM_DS >> 3,    phys_to_virt((u32)(apm_bios_info->apm_dseg << 4)), (apm_bios_info->apm_dseg_len - 1) & 0xffff, 0, 1);
    */

    apm_indirect.offset = apm_bios_info->apm_offset;
    apm_indirect.segment = __APM_CS;

    apm_enable();
    apm_get_power_status();
    apm_show_power_status(&apm_power_status);
    create_kernel_thread("APM", apm_task_start, NULL);
    
    return 0;
}
