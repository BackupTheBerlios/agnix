#ifndef _AGNIX_ADI_PCIBUS_H_
#define _AGNIX_ADI_PCIBUS_H_

#include <agnix/list.h>

//#include <agnix/adi/core/adi_drivers.h>
#include <agnix/adi/buses/adi_pcibus_regs.h>
#include <agnix/adi/buses/adi_pcibus_class.h>
#include <agnix/adi/buses/adi_pcibus_names.h>
#include <agnix/adi/buses/adi_pcibus_irq.h>
#include <agnix/adi/buses/adi_pcibus_define_names.h>
#include <agnix/adi/core/adi_resources.h>


#define PCIDEV_BUS(x)		(u8)(x->dev_bus->bus_nr & 0xFF)
#define PCIDEV_SLOT(x)		(u8)((x->dev_fn >> 3) & 0x1F)
#define PCIDEV_FN_TO_SLOT(x)	(u8)((x >> 3) & 0x1F)
#define PCIDEV_FN(x)		(u8)(x->dev_fn & 0x7)

#define pci_read_config_byte(pcidev, reg, value)	\
    pcidev->dev_bus->bus_ops->pcibus_read_config(pcidev, reg, (u32 *)value, 1)

#define pci_read_config_word(pcidev, reg, value)	\
    pcidev->dev_bus->bus_ops->pcibus_read_config(pcidev, reg, (u32 *)value, 2)

#define pci_read_config_dword(pcidev, reg, value)	\
    pcidev->dev_bus->bus_ops->pcibus_read_config(pcidev, reg, (u32 *)value, 4)

#define pci_write_config_byte(pcidev, reg, value)	\
    pcidev->dev_bus->bus_ops->pcibus_write_config(pcidev, reg, (u32)value, 1)

#define pci_write_config_word(pcidev, reg, value)	\
    pcidev->dev_bus->bus_ops->pcibus_write_config(pcidev, reg, (u32)value, 2)

#define pci_write_config_dword(pcidev, reg, value)	\
    pcidev->dev_bus->bus_ops->pcibus_write_config(pcidev, reg, (u32)value, 4)


struct pcidev_s;
struct adi_driver_pci_s;
struct adi_driver_id_s;

struct pcibus_ops_s {
    int (*pcibus_read_config)(struct pcidev_s *pcidev, int reg, u32 *value, int len);
    int (*pcibus_write_config)(struct pcidev_s *pcidev, int reg, u32 value, int len);
};

struct pcibus_s {
    int bus_nr;
    struct pcibus_ops_s *bus_ops;
    struct list_head bus_list;
};

struct pcidev_s {
    struct pcibus_s *dev_bus;
    const char *dev_name;
    u8  dev_fn;
    u8  dev_irq;
    u8  dev_irq_pin;
    u8  dev_header_type;
    u16 dev_vendor_id;
    u16 dev_device_id;
    u16 dev_subvendor_id;
    u16 dev_subdevice_id;
    u32 dev_class;
    int dev_multi;

    int dev_max_resources;
    struct adi_resource_s dev_resources[PCIDEV_MAX_RESOURCES];
    struct list_head dev_list;
};

int pcibus_ops_init(void);
int pcibus_scan(int bus_nr);
int pcibus_scan_all(void);
int adi_pcibus_devices_init(void);
int adi_pcibus_init(void);

struct pcidev_s *pcibus_find_device_by_id(u16 vendor_id, u16 device_id);
struct pcidev_s *pcibus_find_device_by_fn(u8 bus_nr, int function);
int pcibus_enable_device(struct pcidev_s *pcidev);

int adi_pcibus_device_init(struct adi_driver_pci_s *adi_driver_pci, struct adi_driver_id_s *adi_driver_id);

#endif
