/*
 * kernel_drivers/adi_buses/pci/pcibus_irq.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/06
 * Author: 	Lukasz Dembinski
 * Info:	pcibus_irq.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>
#include <asm/pgtable.h>
#include "pcibus_define_names.h"

#define MOD_NAME				"PCI: \t\t"
#define PCIBUS_ROUTING_TABLE_START		phys_to_virt(0xf0000)
#define PCIBUS_ROUTING_TABLE_END		phys_to_virt(0x100000)

#define pcibus_irq_router_get(pirq, irq_ptr)	pcidev_irq_router_ops->pirq_get(pirq, irq_ptr)
#define pcibus_irq_router_set(pirq, irq)	pcidev_irq_router_ops->pirq_set(pirq, irq)

#define PCI_DEBUG				0

extern struct list_head pcidev_list;
extern spinlock_t 	pcidev_list_lock;

struct pcibus_irq_table *pirq_table;
struct pcidev_s *pcidev_irq_router;
struct pcibus_irq_router_ops *pcidev_irq_router_ops;

int pcibus_irq_piix_get(int pirq, u8 *irq);
int pcibus_irq_piix_set(int pirq, u8 irq);

struct pcibus_irq_router irq_routers[] = {
    { PCIBUS_VENDOR_INTEL_ID, PCIBUS_DEVICE_82371AB_ID, { pcibus_irq_piix_get, pcibus_irq_piix_set } },
    { 0 ,}
};

int pcibus_irq_piix_get(int pirq, u8 *irq)
{
    pci_read_config_byte(pcidev_irq_router, pirq, irq);
    
    return 0;
}

int pcibus_irq_piix_set(int pirq, u8 irq)
{
    pci_write_config_byte(pcidev_irq_router, pirq, irq);
    
    return 0;
}

u8  pcibus_irq_checksum(struct pcibus_irq_table *pirq_table)
{
    u8  *pirq_table_ptr;
    u16 i;
    u8  checksum = 0;
    
    pirq_table_ptr = (u8 *)pirq_table;
    for (i = 0; i < pirq_table->size; i++)
	checksum += pirq_table_ptr[i];
    
    return checksum;
}

struct pcibus_irq_table *pcibus_irq_find_routing_table(void)
{
    u32 table_start = PCIBUS_ROUTING_TABLE_START;
    u32 table_end   = PCIBUS_ROUTING_TABLE_END;
    u32 table_ptr;
    struct pcibus_irq_table *pirq_table;
    
    for (table_ptr = table_start; table_ptr < table_end; table_ptr += 16) {
	pirq_table = (struct pcibus_irq_table *)table_ptr;
	
	if (pirq_table->signature == PCIBUS_PIRQ_SIGNATURE &&
	    pirq_table->version   == PCIBUS_PIRQ_VERSION   && 
	    pirq_table->size       > sizeof(struct pcibus_irq_table)) {
	    
	    if (pcibus_irq_checksum(pirq_table) == 0) {
		printk(MOD_NAME "irq routing table found (at address 0x%08x)\n", virt_to_phys(table_ptr));
		return pirq_table;
	    } else {
	    	printk(MOD_NAME "irq routing table checksum failed! (at address 0x%08x)\n", virt_to_phys(table_ptr));
		return NULL;
	    }
	}
    }
    
    printk(MOD_NAME "irq routing table not found\n");
    
    return NULL;
}

struct pcidev_s *pcibus_irq_find_router(struct pcibus_irq_table *pirq_table)
{
    struct pcidev_s *pcidev_1 = NULL;
    struct pcidev_s *pcidev_2 = NULL;

    if (pirq_table->router_vendor != 0x0000 && pirq_table->router_vendor != 0xffff &&
        pirq_table->router_device != 0x0000 && pirq_table->router_device != 0xffff)
	    pcidev_1 = pcibus_find_device_by_id(pirq_table->router_vendor, pirq_table->router_device);

    pcidev_2 = pcibus_find_device_by_fn(pirq_table->router_bus, pirq_table->router_function);
    
    if (pcidev_1 != NULL && pcidev_2 != NULL && pcidev_1 == pcidev_2)
	return pcidev_1;

    if (pcidev_1 == NULL)
	return pcidev_2;
    
    return pcidev_1;
}

struct pcibus_irq_router_ops *pcibus_irq_find_router_ops(struct pcidev_s *pcidev)
{
    int i = 0;

    for (i = 0; irq_routers[i].router_vendor_id; i++) {
	if (pcidev->dev_vendor_id == irq_routers[i].router_vendor_id &&
	    pcidev->dev_device_id == irq_routers[i].router_device_id)
		return &(irq_routers[i].router_ops);
    }
    
    return NULL;
}

struct pcibus_irq_table_slot *pcibus_irq_find_slot(struct pcidev_s *pcidev)
{
    int i;
    int slots_nr = (pirq_table->size - sizeof(struct pcibus_irq_table)) / sizeof(struct pcibus_irq_table_slot);
    struct pcibus_irq_table_slot *table_slot = 
		  (struct pcibus_irq_table_slot *)(((void *)pirq_table) + sizeof(struct pcibus_irq_table));

    for (i = 0; i <= slots_nr + 2; i++) {
	if ((table_slot[i].dev_bus      		   == pcidev->dev_bus->bus_nr) &&
	    (PCIDEV_FN_TO_SLOT(table_slot[i].dev_function) == PCIDEV_SLOT(pcidev)))
		return &table_slot[i];
    }
        
    return NULL;
}

int pcibus_irq_dev_set(struct pcidev_s *pcidev)
{
    struct pcibus_irq_table_slot *slot;
    struct pcibus_irq_table_slot_irq *slot_irq;    
    struct pcidev_s *pcidev_tmp;
    struct list_head *tmp;
    u8 dev_pin;
    u8 dev_irq;
    u8  router_irq_ptr;
    u16 router_irq_bitmap;
    int sharing_with = 0;
            
    pci_read_config_byte(pcidev, PCIDEV_INTERRUPT_PIN, &dev_pin);

    /* device has not irq pin */
    if (dev_pin == 0) {
	return 0;
    }

    /* some devices has irq = 255 */
    if (pcidev->dev_irq >= 16) {
#if PCI_DEBUG
	printk(MOD_NAME "%02x.%02x has irq >= 16, setting to 0\n", PCIDEV_SLOT(pcidev), PCIDEV_FN(pcidev));
#endif
	pcidev->dev_irq = 0;
    }

    if (pcidev->dev_irq) {
        printk(MOD_NAME "found irq %d for %02x.%02x\n", pcidev->dev_irq, PCIDEV_SLOT(pcidev), PCIDEV_FN(pcidev));
	return 0;
    }
    
    dev_pin--;

    if (dev_pin > PCIDEV_MAX_IRQS) {
#if PCI_DEBUG
    	printk("dev_pin > MAX\n");
#endif
	return 0;
    }

    if ((slot = pcibus_irq_find_slot(pcidev)) == NULL) {
#if PCI_DEBUG
    	printk("slot not found\n");
#endif
	return 0;
    }

    slot_irq = slot->dev_irq;

    router_irq_ptr    = slot_irq[dev_pin].link;
    router_irq_bitmap = slot_irq[dev_pin].bitmap;
    
    pcibus_irq_router_get(router_irq_ptr, &dev_irq);
    pcidev->dev_irq = dev_irq;

    printk(MOD_NAME "found irq %d for %02x.%02x", dev_irq, PCIDEV_SLOT(pcidev), PCIDEV_FN(pcidev));
    
    list_for_each(tmp, &pcidev_list) {
	pcidev_tmp = list_entry(tmp, struct pcidev_s, dev_list);
	
	if (pcidev_tmp == pcidev)
	    continue;
	
	pci_read_config_byte(pcidev_tmp, PCIDEV_INTERRUPT_PIN, &dev_pin);
	
	if (!dev_pin)
	    continue;
	
	dev_pin--;
	
        if ((slot = pcibus_irq_find_slot(pcidev_tmp)) == NULL)
	    continue;
	
	if (slot->dev_irq[dev_pin].link == router_irq_ptr) {
	
	    if (pcidev_tmp->dev_irq >= 16)
		pcidev_tmp->dev_irq = 0;
	
	    if (pcidev_tmp->dev_irq && (pcidev_tmp->dev_irq != pcidev->dev_irq)) {
		printk(", conflict with %02x.%02x", PCIDEV_SLOT(pcidev_tmp), PCIDEV_FN(pcidev_tmp));
		return -1;
	    }
	
	    pcidev_tmp->dev_irq = dev_irq;
	    if (!sharing_with) {
		sharing_with = 1;
		printk(", sharing with");
	    }
	    printk(" %02x.%02x", PCIDEV_SLOT(pcidev_tmp), PCIDEV_FN(pcidev_tmp));
	}
    }
    
    printk("\n");
    
    return 0;
}

int pcibus_irq_all_set(void)
{
    struct list_head *tmp;
    struct pcidev_s *pcidev;
    int ret = 0;
    u32 flags;

    spin_lock_irqsave(&pcidev_list_lock, flags);
    
    list_for_each(tmp, &pcidev_list) {
	pcidev = list_entry(tmp, struct pcidev_s, dev_list);
	if ((ret = pcibus_irq_dev_set(pcidev)) < 0) 
	    break;
    }    
    
    spin_unlock_irqrestore(&pcidev_list_lock, flags);
    
    return ret;
}

int pcibus_irq_init(void)
{
    pirq_table 		  = pcibus_irq_find_routing_table();
    pcidev_irq_router 	  = pcibus_irq_find_router(pirq_table);
    pcidev_irq_router_ops = pcibus_irq_find_router_ops(pcidev_irq_router);

    if (pcidev_irq_router != NULL)
	printk(MOD_NAME "found %s irq router\n", pcidev_irq_router->dev_name);
    else
	printk(MOD_NAME "irq router not found\n");

    printk(MOD_NAME "setting pirqs.... \n");

    if (!pcibus_irq_all_set())
	return -1;

    return 0;
}
