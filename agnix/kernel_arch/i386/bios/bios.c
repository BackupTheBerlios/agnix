/*
 * kernel_arch/i386/kernel/bios/bios.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	bios.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/bios/bios.h>
 
//int smbios_init(void);

int __init bios_init(void)
{
    smbios_init();

//#ifdef CONFIG_BIOS_APM
//    apm_init();    
//#endif

}
