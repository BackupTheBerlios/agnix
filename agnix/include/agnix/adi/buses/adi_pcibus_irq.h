#ifndef _AGNIX_ADI_PCIBUS_IRQ_H_
#define _AGNIX_ADI_PCIBUS_IRQ_H_

#define PCIDEV_MAX_IRQS		4
#define PCIBUS_PIRQ_SIGNATURE	('$' | ('P' << 8) | ('I' << 16) | ('R' << 24))
#define PCIBUS_PIRQ_VERSION	0x100

struct pcibus_irq_router_ops {
    int (*pirq_get)(int pirq, u8 *irq);
    int (*pirq_set)(int pirq, u8  irq);
};
 
struct pcibus_irq_router {
    u16 router_vendor_id;
    u16 router_device_id;
    struct pcibus_irq_router_ops router_ops;
};

struct pcibus_irq_table_slot_irq {
    u8  link;
    u16 bitmap;
} __attribute__((packed));

struct pcibus_irq_table_slot {
    u8  dev_bus;
    u8  dev_function;
    struct pcibus_irq_table_slot_irq dev_irq[PCIDEV_MAX_IRQS];
    u8  dev_slot;
    u8  dev_rfu;
};

struct pcibus_irq_table {
    u32 signature;
    u16 version;
    u16 size;
    u8  router_bus;    
    u8  router_function;
    u16 exclusive_irq;
    u16 router_vendor;
    u16 router_device;
    u32 miniport_data;
    u8  rfu[11];
    u8  checksum;
};

int pcibus_irq_init(void);

#endif
