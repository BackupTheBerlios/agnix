/*
 * kernel_drivers/adi_buses/pci/pcibus_scan.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/06
 * Author: 	Lukasz Dembinski
 * Info:	pcibus_scan.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/memory.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>

#define MOD_NAME	"PCI: \t\t"
#define PCI_DEBUG	1

extern struct list_head pcibus_list;
extern struct list_head pcidev_list;
extern spinlock_t pcibus_list_lock;
extern spinlock_t pcidev_list_lock;

struct pcibus_s *pcibus_primary;
extern struct pcibus_ops_s pcibus_primary_ops;
extern int pcibus_disable;

void pcibus_add_device(struct pcidev_s *pcidev)
{
    u32 flags;

    spin_lock_irqsave(&pcidev_list_lock, flags);
    list_add_tail(&(pcidev->dev_list), &pcidev_list);
    spin_unlock_irqrestore(&pcidev_list_lock, flags);
}

void pcibus_add_bus(struct pcibus_s *pcibus)
{
    u32 flags;

    spin_lock_irqsave(&pcibus_list_lock, flags);
    list_add_tail(&(pcibus->bus_list), &pcibus_list);
    spin_unlock_irqrestore(&pcibus_list_lock, flags);
}

struct pcidev_s *pcibus_find_device_by_id(u16 vendor_id, u16 device_id)
{
    u32 flags;
    struct list_head *tmp;
    struct pcidev_s *pcidev;

    spin_lock_irqsave(&pcidev_list_lock, flags);
    
    list_for_each(tmp, &pcidev_list) {
	pcidev = list_entry(tmp, struct pcidev_s, dev_list);
	
	if (pcidev->dev_vendor_id == vendor_id &&
	    pcidev->dev_device_id == device_id)
		goto pcidev_found;
    }
    
    pcidev = NULL;
    
pcidev_found:
    spin_unlock_irqrestore(&pcidev_list_lock, flags);

    return pcidev;
}

struct pcidev_s *pcibus_find_device_by_fn(u8 bus_nr, int function)
{
    u32 flags;
    struct list_head *tmp;
    struct pcidev_s *pcidev;

    spin_lock_irqsave(&pcidev_list_lock, flags);
    
    list_for_each(tmp, &pcidev_list) {
	pcidev = list_entry(tmp, struct pcidev_s, dev_list);
	
	if (pcidev->dev_fn == function && pcidev->dev_bus->bus_nr == bus_nr)
		goto pcidev_found;
    }
    
    pcidev = NULL;
    
pcidev_found:
    spin_unlock_irqrestore(&pcidev_list_lock, flags);

    return pcidev;
}

int pcibus_find_capability_pos(struct pcidev_s *pcidev, int cap_req_id)
{
    u16 dev_status;
    u8  cap_next;
    u8  cap_pos;
    u8  cap_id;
    int cap_max_entries = 48;
    
    pci_read_config_word(pcidev, PCI_STATUS, &dev_status);
    
    if (!(dev_status & PCI_STATUS_CAP_LIST))
	return -1;

    switch(pcidev->dev_header_type) {
	case PCIDEV_HDR_TYPE_NORMAL:
	case PCIDEV_HDR_TYPE_BRIDGE:
	    cap_pos = PCI_CAPABILITY_LIST;
	case PCIDEV_HDR_TYPE_CB:
	    cap_pos = PCI_CB_CAPABILITY_LIST;
	default:
	    return -1;
    }
    
    for(cap_max_entries = 48; cap_max_entries > 0; cap_max_entries--) {
	pci_read_config_byte(pcidev, cap_pos + PCI_CAP_LIST_ID, &cap_id);
	
	if (cap_id == cap_req_id)
	    return cap_pos;
	
	if (cap_id == 0xff)
	    break;
	    
	pci_read_config_byte(pcidev, cap_pos + PCI_CAP_LIST_NEXT, &cap_next);
	
	if (cap_pos <= 0x40)
	    return -1;
	
	cap_pos = cap_next;
    }

    return -1;    
}

int pcibus_power_change_delay(struct pcidev_s *pcidev, int power_1, int power_2)
{	
    return 0;
}

int pcibus_set_power_state(struct pcidev_s *pcidev, int power_state)
{
    int cap_pos;
    u16 pm_caps;
    u16 pm_ctrl;

    if (power_state < 0 || power_state > 3)
	return -1;
    
    if ((cap_pos = pcibus_find_capability_pos(pcidev, PCI_CAP_ID_PM)) < 0)
	return -1;

//    printk("cap_pos = %d\n", cap_pos);
    
    if (power_state == 1 || power_state == 2) {
        pci_read_config_word(pcidev, cap_pos + PCI_PM_PMC, &pm_caps);
	if (!(pm_caps & (PCI_PM_CAP_D1 << (power_state - 1))))
	    return -1;
    }

    pci_read_config_word(pcidev, cap_pos + PCI_PM_CTRL, &pm_ctrl);
//    printk("pm_ctrl = %02x\n", pm_ctrl);
    pm_ctrl = (pm_ctrl & (~PCI_PM_CTRL_STATE_MASK)) | power_state;
//    printk("pm_ctrl = %02x\n", pm_ctrl);
    pci_write_config_word(pcidev, cap_pos + PCI_PM_CTRL, pm_ctrl);

    pcibus_power_change_delay(pcidev, 0, 0);

    return 0;
}

int pcibus_enable_device(struct pcidev_s *pcidev)
{
    u16 pcidev_cmd_tmp;
    u16 pcidev_cmd;
    int i;

    pcibus_set_power_state(pcidev, 0);
    
    pci_read_config_word(pcidev, PCI_COMMAND, &pcidev_cmd);
    pcidev_cmd_tmp = pcidev_cmd;
    
    for (i = 0; i < pcidev->dev_max_resources; i++) {
	if (pcidev->dev_resources[i].res_flags & ADI_RES_FLAG_IO)
	    pcidev_cmd |= PCI_COMMAND_IO;
	else
	if (pcidev->dev_resources[i].res_flags & ADI_RES_FLAG_MEM)
	    pcidev_cmd |= PCI_COMMAND_MEMORY;
    }
    
    if (pcidev_cmd != pcidev_cmd_tmp) 
	pci_write_config_word(pcidev, PCI_COMMAND, pcidev_cmd);

    return 0;
}

struct pcibus_s *pcibus_allocate_primary_bus(int bus_nr, struct pcibus_ops_s *bus_ops)
{
    struct pcibus_s *pcibus;

    if (bus_ops == NULL || bus_ops->pcibus_read_config == NULL 
                        || bus_ops->pcibus_write_config == NULL) {
	printk(MOD_NAME "invalid bus_ops\n");
	return NULL;
    }

    pcibus = (struct pcibus_s *)get_free_pages(0);
    pcibus->bus_nr  = bus_nr;
    pcibus->bus_ops = bus_ops;

    pcibus_add_bus(pcibus);

    return pcibus;
}

struct pcibus_s *pcibus_allocate_child_bus(struct pcibus_s *parent, int child_bus_nr)
{
    struct pcibus_s *child;

    child = (struct pcibus_s *)get_free_pages(0);

    child->bus_nr  = child_bus_nr;
    child->bus_ops = parent->bus_ops;

    return child;
}

int pcibus_scan_card_memory(struct pcidev_s *pcidev, unsigned int res_nr)
{
    u32 start;
    u32 size;
    u32 value;
    unsigned int i;
    unsigned int offset = 0;
    struct adi_resource_s *adi_resource;

    for (i = 0; i < res_nr; i++) {
	
	/* PCI Local Bus Specification 2.2 */
	offset = PCIDEV_BASE_ADDRESS_0 + (i << 2);
	pci_read_config_dword (pcidev, offset, &value);
	pci_write_config_dword(pcidev, offset, 0xFFFFFFFF);
	pci_read_config_dword (pcidev, offset, &size);
	pci_write_config_dword(pcidev, offset, value);
    
	if (size == 0x00000000 || size == 0xFFFFFFFF)
	    continue;

	if (value == 0xFFFFFFFF) value = 0;

	adi_resource = &pcidev->dev_resources[i];

	if ((value & PCIDEV_BASE_ADDRESS_MASK) == PCIDEV_BASE_ADDRESS_MEMORY) {
	    start = value & PCIDEV_BASE_ADDRESS_START_MASK_MEM;
	    size &= PCIDEV_BASE_ADDRESS_START_MASK_MEM;
	    size = size & (~(size - 1));
	    size--;
	    
	    adi_resource->res_flags = ADI_RES_FLAG_MEM;
//	    printk(MOD_NAME "%d %08x-%08x memory ", i, start, start + size);
//	    if ((value & PCIDEV_BASE_ADDRESS_TYPE_MASK) == PCIDEV_BASE_ADDRESS_TYPE_32BIT)
//		printk(MOD_NAME "32-bit\n");
//	    else
//	    if ((value & PCIDEV_BASE_ADDRESS_TYPE_MASK) == PCIDEV_BASE_ADDRESS_TYPE_64BIT)
//	    	printk(MOD_NAME "64-bit\n");

	} else
	if ((value & PCIDEV_BASE_ADDRESS_MASK) == PCIDEV_BASE_ADDRESS_IO) {
	    start = value & PCIDEV_BASE_ADDRESS_START_MASK_IO;
	    size &= PCIDEV_BASE_ADDRESS_START_MASK_IO;
	    size = size & (~(size - 1));
	    size--;
	    
//	    printk(MOD_NAME "%d %04x-%04x io \n", i, start, start + size);
//	    adi_resource->res_start = start;
//	    adi_resource->res_end = start + size;
	    adi_resource->res_flags = ADI_RES_FLAG_IO;
	}
	
	adi_resource->res_name = pcidev->dev_name;
	adi_resource->res_start = start;
	adi_resource->res_end = start + size;

        INIT_LIST_HEAD(&(adi_resource->res_list));
        if (adi_register_resource(adi_resource) < 0) {
	    printk(MOD_NAME "Can not register resource\n");
        }
    }
    
    return 0;
}

int pcibus_scan_card_function(struct pcidev_s *pcidev)
{
    u32 dev_class;
    u8  dev_irq_pin;
    u8  dev_irq;
    u16 dev_subvendor_id;
    u16 dev_subdevice_id;

    pci_read_config_dword(pcidev, PCIDEV_CLASS_REVISION,  &dev_class);
    pci_read_config_byte (pcidev, PCIDEV_INTERRUPT_PIN,   &dev_irq_pin);
    pci_read_config_byte (pcidev, PCIDEV_INTERRUPT_LINE,  &dev_irq);
    pci_read_config_word (pcidev, PCIDEV_SUBSYSTEM_ID,    &dev_subdevice_id);
    pci_read_config_word (pcidev, PCIDEV_SUBSYSTEM_VENDOR_ID, &dev_subvendor_id);

    pcidev->dev_class 	= dev_class >> 8;
    pcidev->dev_irq_pin = dev_irq_pin;
    pcidev->dev_irq 	= dev_irq;
    pcidev->dev_subvendor_id = dev_subvendor_id;
    pcidev->dev_subdevice_id = dev_subdevice_id;

    if ((u16)(pcidev->dev_class >> 8) == PCI_CLASS_BRIDGE_CARDBUS) {
#if PCI_DEBUG
	printk(MOD_NAME "%s cardbus legacy mode base\n", pcidev->dev_name);
#endif
	pci_write_config_dword(pcidev, PCIDEV_CB_LEGACY_MODE_BASE, 0);
    }

    switch (pcidev->dev_header_type) {
	case PCIDEV_HDR_TYPE_NORMAL:
	    pcibus_scan_card_memory(pcidev, PCIDEV_MAX_RESOURCES);
	    break;

	case PCIDEV_HDR_TYPE_BRIDGE:
	    pcibus_scan_card_memory(pcidev, PCIDEV_BRIDGE_RESOURCES);
	    break;

	case PCIDEV_HDR_TYPE_CB:
	    pcibus_scan_card_memory(pcidev, PCIDEV_CB_RESOURCES);
	    break;

	default:
	    printk(MOD_NAME "%s wrong header type\n", pcidev->dev_name);
	    return -1;
    }
    
    return 0;
}

int pcibus_scan_card(struct pcidev_s *pcidev)
{
    u8  dev_fn;
//    int dev_fn_saved;
    u8  dev_fn_cur = 0;
    int dev_multi  = 0;
    struct pcidev_s *detected_dev;
    u16 dev_vendor_id;
    u16 dev_device_id;
    u8  dev_header_type;
//    u8  i, b;

//    dev_fn_saved = pcidev->dev_fn;
//    dev_fn = pcidev->dev_fn;

    //for (dev_fn = dev_fn_saved; dev_fn < dev_fn_saved + 8; dev_fn++, dev_fn_cur++) {
    for (dev_fn = pcidev->dev_fn; dev_fn_cur < 8; dev_fn++, dev_fn_cur++) {
	if ((!dev_multi) && dev_fn_cur)
	    break;
	    
	pcidev->dev_fn = dev_fn;

        pci_read_config_byte(pcidev, PCIDEV_HEADER_TYPE, &dev_header_type);
        pci_read_config_word(pcidev, PCIDEV_VENDOR_ID, &dev_vendor_id);
        pci_read_config_word(pcidev, PCIDEV_DEVICE_ID, &dev_device_id);

	if ((dev_vendor_id == 0x0000 && ((dev_device_id == 0xFFFF) || (dev_device_id == 0x0000))) || 
	    (dev_vendor_id == 0xffff))
	    return -1;
	    
	detected_dev = (struct pcidev_s *)get_free_pages(0);

	if (detected_dev == NULL) {
#if PCI_DEBUG	
	    printk(MOD_NAME "can not allocate memory for PCI dev\n");
#endif
	    return -1;
	}
	
	memcpy(detected_dev, pcidev, sizeof(struct pcidev_s));

	if ((dev_header_type & 0x80) && (!dev_fn_cur))
	    dev_multi = 1;

	detected_dev->dev_header_type  = dev_header_type & 0x7F;
	detected_dev->dev_vendor_id    = dev_vendor_id;
	detected_dev->dev_device_id    = dev_device_id;
	detected_dev->dev_name         = pcibus_device_name(dev_vendor_id, dev_device_id);
	detected_dev->dev_multi        = dev_multi;

	pcibus_scan_card_function(detected_dev);

#if PCI_DEBUG	
	printk(MOD_NAME "detected %s (%04x, %04x)\n", detected_dev->dev_name, dev_vendor_id, dev_device_id);
#endif

	pcibus_add_device(detected_dev);
    }

    return 0;
}

int pcibus_scan(int bus_nr)
{
    printk(MOD_NAME "scanning PCI bus %d\n", bus_nr);

    return 0;
}

int pcibus_scan_all(void)
{
    struct pcidev_s pcidev;
    u8 dev_fn;
    int i;

    printk(MOD_NAME "scanning all PCI buses\n");

    pcibus_primary = pcibus_allocate_primary_bus(0, &pcibus_primary_ops);

    if (pcibus_primary == NULL) {
	printk(MOD_NAME "can not allocate memory for primary PCI bus, disabling PCI subsystem\n");
	pcibus_disable = 1;
	return 0;
    }

    pcidev.dev_bus = pcibus_primary;
    
    for (i = 0, dev_fn = 0; i < 32; dev_fn += 8, i++) {
	pcidev.dev_fn = dev_fn;
	
	pcibus_scan_card(&pcidev);
    }

//    adi_print_io_resources();
//    adi_print_mem_resources();

    return 0;
}
