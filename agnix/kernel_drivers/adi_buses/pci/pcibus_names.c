/*
 * kernel_drivers/adi_buses/pci/pcibus_names.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/06
 * Author: 	Lukasz Dembinski
 * Info:	pcibus_names.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */
 
#include <agnix/agnix.h>
#include <agnix/adi/adi.h>

extern struct pcibus_vendor_name_s pcibus_vendor_names[];
extern struct pcibus_device_name_s pcibus_device_names[];

const char *pcibus_device_name(u32 vendor_id, u32 device_id)
{
    int i = 0;

    while(pcibus_device_names[i].device_name != NULL) {
	if ((pcibus_device_names[i].vendor_id == vendor_id) &&
	    (pcibus_device_names[i].device_id == device_id)) {
		return pcibus_device_names[i].device_name;
	}
    
	i++;
    }
    
    return NULL;
}

const char *pcibus_vendor_name(u32 vendor_id)
{
    int i = 0;

    while(pcibus_vendor_names[i].vendor_name != NULL) {
	if (pcibus_vendor_names[i].vendor_id == vendor_id) {
	    return pcibus_vendor_names[i].vendor_name;
	}
    
	i++;
    }
    
    return NULL;
}
