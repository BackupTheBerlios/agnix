/*
 * kernel_drivers/adi_buses/pci/pcibus_ops.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/06
 * Author: 	Lukasz Dembinski
 * Info:	pcibus_ops.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/ioport.h>
#include <agnix/console.h>

#define MOD_NAME	"PCI: \t\t"

#define PCIBUS_ADDRESS_1(bus, device, fn, reg) 	((u32)(0x80000000 | (bus << 16) | (device << 11) | (fn << 8) | (reg & ~3)))
#define PCIBUS_ADDRESS_2(device, reg) 		((u16)(0xC000 | (device << 8) | (reg & 0xFF)))

#define PCIBUS_CONF_ADDR_1	0xCF8
#define PCIBUS_CONF_DATA_1	0xCFC

#define PCIBUS_CONF_FN_2	0xCF8
#define PCIBUS_CONF_BUS_2	0xCFA

struct pcibus_ops_s pcibus_primary_ops;

int pcibus_direct_send_address_1(struct pcidev_s *pcidev, int reg)
{
    int dev_bus  = PCIDEV_BUS(pcidev);
    int dev_slot = PCIDEV_SLOT(pcidev);
    int dev_fn   = PCIDEV_FN(pcidev);  

    if (reg < 0 || reg > 255) {
	printk(MOD_NAME "wrong values in pcibus_direct_read_1 (bus=%02x slot=%02x fn=%01x reg=%d\n", 
	       dev_bus, dev_slot, dev_fn, reg);
	return -1;
    }

    outl(PCIBUS_ADDRESS_1(dev_bus, dev_slot, dev_fn, reg), PCIBUS_CONF_ADDR_1);
    
    return 0;
}

int pcibus_read_config_1(struct pcidev_s *pcidev, int reg, u32 *value, int len)
{
    pcibus_direct_send_address_1(pcidev, reg);
    
    switch(len) {
	case 4: *value = inl(PCIBUS_CONF_DATA_1);
		break;
	case 2: *(u16 *)value = inw(PCIBUS_CONF_DATA_1 + (reg & 0x02)); 
		break;
	case 1: *(u8  *)value = inb(PCIBUS_CONF_DATA_1 + (reg & 0x03));
    }
        
    return 0;    
}

int pcibus_write_config_1(struct pcidev_s *pcidev, int reg, u32 value, int len)
{
    pcibus_direct_send_address_1(pcidev, reg);
    
    switch(len) {
	case 4: outl(value, PCIBUS_CONF_DATA_1);
		break;
	case 2: outw((u16)(value), PCIBUS_CONF_DATA_1 + (reg & 0x02));
		break;
	case 1: outb((u8) (value), PCIBUS_CONF_DATA_1 + (reg & 0x03));
    }
        
    return 0;    
}

int pcibus_direct_send_address_2(struct pcidev_s *pcidev, int reg)
{
    int dev_bus  = PCIDEV_BUS(pcidev);
    int dev_slot = PCIDEV_SLOT(pcidev);
    int dev_fn   = PCIDEV_FN(pcidev);  

    if (reg > 255) {
	printk(MOD_NAME "wrong values in pcibus_direct_read_1 (bus=%02x slot=%02x fn=%01x reg=%02x\n", 
	       dev_bus, dev_slot, dev_fn, reg);
	return -1;
    }

    outl(0xF0 | (dev_fn << 1), PCIBUS_CONF_BUS_2);
    outl(dev_bus, PCIBUS_CONF_BUS_2);
    
    return 0;
}

int pcibus_read_config_2(struct pcidev_s *pcidev, int reg, u32 *value, int len)
{
    int dev_slot = PCIDEV_SLOT(pcidev);

    pcibus_direct_send_address_2(pcidev, reg);
    
    switch(len) {
	case 4: *value = inl(PCIBUS_ADDRESS_2(dev_slot, reg));
		break;
	case 2: *(u16 *)value = inw(PCIBUS_ADDRESS_2(dev_slot, reg));
		break;
	case 1: *(u8  *)value = inb(PCIBUS_ADDRESS_2(dev_slot, reg));
    }
        
    return 0;    
}

int pcibus_write_config_2(struct pcidev_s *pcidev, int reg, u32 value, int len)
{
    int dev_slot = PCIDEV_SLOT(pcidev);

    pcibus_direct_send_address_2(pcidev, reg);
    
    switch(len) {
	case 4: outl((u32)(value), PCIBUS_ADDRESS_2(dev_slot, reg));
		break;
	case 2: outw((u16)(value), PCIBUS_ADDRESS_2(dev_slot, reg));
		break;
	case 1: outb((u8) (value), PCIBUS_ADDRESS_2(dev_slot, reg));
    }
        
    return 0;    
}

int pcibus_check_direct_access_1(void)
{
    u32 saved_value;
    u32 write_value = 0x80000000;
    u32 read_value;

    outb(0x01, 0xCFB);    
    saved_value = inl(PCIBUS_CONF_ADDR_1);
    
    outl(write_value, PCIBUS_CONF_ADDR_1);
    read_value = inl(PCIBUS_CONF_ADDR_1);
    
    if (read_value == write_value) {
	outl(saved_value, PCIBUS_CONF_ADDR_1);
	printk(MOD_NAME "detected type 1 direct access to PCI configuration space\n");
	return 0;
    }
    
    outl(saved_value, PCIBUS_CONF_ADDR_1);
    
    return -1;
}

int pcibus_check_direct_access_2(void)
{
    u8 saved_bus;
    u8 saved_fn;
    u8 read_bus;
    u8 read_fn;
    
    outb(0x00, 0xCFB);    
    
    saved_fn  = inb(PCIBUS_CONF_FN_2);
    saved_bus = inb(PCIBUS_CONF_BUS_2);

    outb(0x00, PCIBUS_CONF_FN_2);
    outb(0x00, PCIBUS_CONF_BUS_2);

    read_fn  = inb(PCIBUS_CONF_FN_2);
    read_bus = inb(PCIBUS_CONF_BUS_2);

    if (read_fn == 0x00 && read_bus == 0x00) {
    	outl(saved_fn, PCIBUS_CONF_FN_2);
	outl(saved_bus, PCIBUS_CONF_BUS_2);
	printk(MOD_NAME "detected type 2 direct access to PCI configuration space\n");
	return 0;
    }
    
    return -1;
}

int pcibus_check_direct_access(void)
{
    if (pcibus_check_direct_access_1() == 0) {
	pcibus_primary_ops.pcibus_read_config  = pcibus_read_config_1;
	pcibus_primary_ops.pcibus_write_config = pcibus_write_config_1;
	return 0;
    } else
    if (pcibus_check_direct_access_2() == 0)
	pcibus_primary_ops.pcibus_read_config  = pcibus_read_config_2;
	pcibus_primary_ops.pcibus_write_config = pcibus_write_config_2;
	return 0;
    
    return -1;
}

int pcibus_ops_init(void)
{
    printk(MOD_NAME "initializing pcibus_ops\n");

    if (pcibus_check_direct_access() < 0) {
	printk(MOD_NAME "can not detect access to PCI configuration space\n");
	return -1;
    }

    return 0;
}
