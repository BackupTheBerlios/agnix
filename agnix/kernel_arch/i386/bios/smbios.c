/*
 * kernel_arch/i386/kernel/bios/smbios.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	smbios.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/bios/bios.h>
#include <agnix/console.h>
#include <agnix/memory.h>
#include <asm/paging.h>

#define MOD_NAME	"SMBIOS: "

u8 count_sum(char *buf, int len)
{
    int i;
    u8 sum = 0;
    
    for (i = 0; i < len; i++)
	sum += buf[i];
    
    return sum;
}

int __init smbios_find_table(void)
{
    u32 range_cur = 0xF0000;
    u32 range_end = 0xFFFFF;
    struct smbios_eps_s *eps;
    
    do {
        eps = (struct smbios_eps_s *)phys_to_virt(range_cur);

	if (!memcmp(eps->ident, _SM_, 4)) {
	    if (count_sum((char *)eps, sizeof(struct smbios_eps_s))) {
		printk(MOD_NAME "invalid checksum!!!\n");
		return -1;
	    } else {
		printk(MOD_NAME "Found SMBIOS ver. %d.%d\n", eps->smbios_major, eps->smbios_minor);
		return 0;
	    }
	}

	range_cur += 16;
    } while(range_cur < range_end);
    
    return -1;
}

int __init smbios_init(void)
{
    smbios_find_table();        

    return 0;
}
