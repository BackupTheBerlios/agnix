/*
 * kernel_drivers/adi_net/rtl_8029.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/08
 * Author: 	Lukasz Dembinski
 * Info:	rtl_8029.c
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/adi/adi.h>
#include <agnix/ioport.h>
#include <agnix/console.h>
#include <agnix/irq.h>
#include <agnix/memory.h>
#include <agnix/net/net.h>
#include <agnix/counters.h>

#define MOD_NAME	"NET_RTL_8029: "

#define RTL_8029_MAX_RESET_TIME	10

/*
 * RTL 8029 read registers PAGE0
 */
#define RTL_8029_CR		0x00
#define RTL_8029_CLDA0		0x01
#define RTL_8029_CLDA1		0x02
#define RTL_8029_BNRY		0x03
#define RTL_8029_TSR		0x04
#define RTL_8029_NCR		0x05
#define RTL_8029_FIFO		0x06
#define RTL_8029_ISR		0x07
#define RTL_8029_CRDA0		0x08
#define RTL_8029_CRDA1		0x09
#define RTL_8029_RSR		0x0c
#define RTL_8029_CNTR0		0x0d
#define RTL_8029_CNTR1		0x0e
#define RTL_8029_CNTR2		0x0f

#define RTL_8029_DMADATA	0x10
#define RTL_8029_RESET		0x1f

/*
 * RTL 8029 write registers PAGE0
 */
#define RTL_8029_PSTART		0x01
#define RTL_8029_PSTOP		0x02
#define RTL_8029_TPSR		0x03
#define RTL_8029_TBCR0		0x04
#define RTL_8029_TBCR1		0x05
#define RTL_8029_RSAR0		0x08
#define RTL_8029_RSAR1		0x09
#define RTL_8029_RBCR0		0x0a
#define RTL_8029_RBCR1		0x0b
#define RTL_8029_RCR		0x0c
#define RTL_8029_TCR		0x0d
#define RTL_8029_DCR		0x0e
#define RTL_8029_IMR		0x0f

/*
 * RTL 8029 CR bits
 */
#define CR_STP			0x01
#define CR_STA			0x02
#define CR_TXP			0x04
#define CR_READ_DMA		0x08
#define CR_WRITE_DMA		0x10
#define CR_ABORT_DMA		0x20
#define CR_PAGE0		0x00
#define CR_PAGE1		0x40
#define CR_PAGE2		0x80

/*
 * RTL 8029 ISR bits
 */
#define ISR_RESET		0x80
#define ISR_PRX			0x01

/*
 * RTL 8029 IMR bits
 */
#define IMR_PRXE		0x01
#define IMR_PTXE		0x02
#define IMR_RXEE		0x04
#define IMR_TXEE		0x08
#define IMR_OVWE		0x10
#define IMR_CNTE		0x20
#define IMR_RDCE		0x40

/*
 * RTL 8029 RCR bits
 */
#define RCR_BROADCAST		0x04
#define RCR_MULTICAST		0x08
#define RCR_PROMISC		0x10


/*
 * RTL 8029 read registers PAGE1
 */

#define RTL_8029_CURR		0x07 

/*
 * RTL 8029 read registers PAGE1
 */

#define RTL_8029_CURR		0x07 

/*
 * RTL_8029 transmit pages
 * 12 pages = 2 ethernet frames max
 */ 
#define RTL_8029_TX_PAGES	12

/*
 * ********
 */
#define NET_RECV_MODE_BROADCAST	0x01
#define NET_RECV_MODE_MULTICAST	0x02
#define NET_RECV_MODE_PROMISC	0x04

#define RTL_8029_DEBUG		0

extern int counter_jiffies_desc;

u32 rtl_iobase;
u8  rtl_hwaddr[6];
u8  rtl_eprom[32];

struct rtl_8029_rx_desc_s {
    u8  status;
    u8  next;
    u16 count;
};

struct adi_driver_id_s rtl_8029_driver_id[] = {
    { PCIBUS_VENDOR_REALTEK_ID, PCIBUS_DEVICE_RTL_8029_AS__ID },
    { 0 }    
};

int rtl_8029_reset(struct pcidev_s *pcidev)
{
    u32 start_jiffies;
    u32 jiffies_latch[2];
    
    counter_read(counter_jiffies_desc, jiffies_latch);
    start_jiffies = jiffies_latch[0];
    
    outb(inb(rtl_iobase + RTL_8029_RESET), rtl_iobase + RTL_8029_RESET);
    
    while(!(inb(rtl_iobase + RTL_8029_ISR) & ISR_RESET)) {
        counter_read(counter_jiffies_desc, jiffies_latch);
	if (jiffies_latch[0] - start_jiffies >= RTL_8029_MAX_RESET_TIME) {
	    printk(MOD_NAME "reset failure\n");
	    return -1;
	}
    }
    
    outb(0xff, rtl_iobase + RTL_8029_ISR);
    
    return 0;
}

int rtl_8029_set_recv_mode(struct netdev_s *netdev, int recv_mode)
{
    u32 rtl_8029_rcr = 0;

    if (recv_mode & NET_RECV_MODE_BROADCAST) {
	rtl_8029_rcr |= RCR_BROADCAST;
    }
    
    if (recv_mode & NET_RECV_MODE_MULTICAST) {
	rtl_8029_rcr |= RCR_MULTICAST;
    }
    
    if (recv_mode & NET_RECV_MODE_PROMISC) {
	rtl_8029_rcr |= RCR_PROMISC;
    }

    outb(rtl_8029_rcr, rtl_iobase + RTL_8029_RCR);

    return 0;
}

int rtl_8029_open(void)
{
    return 0;
}

int rtl_8029_receive_frame(struct frb_s *frb)
{
    struct rtl_8029_rx_desc_s rx_desc;
    u8 *ptr;
    u8 current_page;
    u8 boundary_page;
    u8 next_page;
    u16 frame_len;
    int i;

    outb(CR_PAGE1 | CR_ABORT_DMA | CR_STP, rtl_iobase + RTL_8029_CR);    
    current_page  = inb(rtl_iobase + 0x07);
    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STP, rtl_iobase + RTL_8029_CR);    

    boundary_page = inb(rtl_iobase + RTL_8029_BNRY) + 1;

#if RTL_8029_DEBUG
    printk("boundary = %02x\n", boundary_page);
#endif

    if (boundary_page >= 0x80)
	boundary_page = 0x40 + RTL_8029_TX_PAGES;

    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STA, rtl_iobase + RTL_8029_CR);
    outb(sizeof(rx_desc), rtl_iobase + RTL_8029_RBCR0);	// lo
    outb(0x00, rtl_iobase + RTL_8029_RBCR1);		// hi
    outb(0x00, rtl_iobase + RTL_8029_RSAR0);
    outb(boundary_page, rtl_iobase + RTL_8029_RSAR1);
    outb(CR_PAGE0 | CR_READ_DMA | CR_STA, rtl_iobase + RTL_8029_CR);

    ptr = (u8 *)&rx_desc;
    *(u32 *)ptr = inl(rtl_iobase + RTL_8029_DMADATA);

    frame_len = rx_desc.count - sizeof(rx_desc);
    frb->frb_len = frame_len;

#if RTL_8029_DEBUG
    printk("frame_len = %d\n", frame_len);
#endif

    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STA, rtl_iobase + RTL_8029_CR);
    outb(frame_len & 0xff, rtl_iobase + RTL_8029_RBCR0);	// lo
    outb(frame_len >> 8,   rtl_iobase + RTL_8029_RBCR1);	// hi
    outb(sizeof(rx_desc), rtl_iobase + RTL_8029_RSAR0);
    outb(boundary_page, rtl_iobase + RTL_8029_RSAR1);
    outb(CR_PAGE0 | CR_READ_DMA | CR_STA, rtl_iobase + RTL_8029_CR);

    ptr = frb->frb_data;

    for (i = 0; i < (frame_len >> 2); i++) {
        *(u32 *)ptr = inl(rtl_iobase + RTL_8029_DMADATA);
	ptr += 4;
    }

    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STA, rtl_iobase + RTL_8029_CR);

    next_page = rx_desc.next;

#if RTL_8029_DEBUG
    printk("next_page = %02x\n\n", next_page);
#endif
    
    outb(next_page - 1, rtl_iobase + RTL_8029_BNRY);
    outb(0xff, rtl_iobase + RTL_8029_ISR);

    return 0;
}

void rtl_8029_irq(u32 data)
{
    u8 irq_status;

    irq_status = inb(rtl_iobase + RTL_8029_ISR);
    
    if (irq_status & ISR_PRX) {
    // tu trzeba zrobic alokowanie bufora na ramke i 
    // nie przetwarzanie jej w przerwaniu tylko po wyjsciu z przerwania
    
/////////////	struct frb_s frb *frame_buf = (u8 *)get_free_pages(0);
	
//	rtl_8029_receive_frame(frame_buf);

//	printk("recv frame %02x %02x %02x %02x %02x %02X -> ", frame_buf[6], frame_buf[7], frame_buf[8], frame_buf[9], frame_buf[10], frame_buf[11]);
//	printk("%02x %02x %02x %02x %02x %02x\n", frame_buf[0], frame_buf[1], frame_buf[2], frame_buf[3], frame_buf[4], frame_buf[5]);

//	put_free_pages((u32)frame_buf, 0);
    }
    
    outb(0xff, rtl_iobase + RTL_8029_ISR);
}

int rtl_8029_open_driver(struct netdev_s *netdev)
{
    return 0;
}

int rtl_8029_close_driver(struct netdev_s *netdev)
{
    return 0;
}

struct irq_routine_s rtl_8029_irq_routine = {
    .proc	= rtl_8029_irq,
    .flags	= IRQ_FLAG_RANDOM,
};

struct netdev_ops_s rtl_8029_netdev_ops = {
    .driver_open 	= rtl_8029_open_driver,
    .driver_close	= rtl_8029_close_driver,
    .set_recv_mode	= rtl_8029_set_recv_mode, 
};

struct netdev_s rtl_8029_netdev = {
    .if_type		= IF_TYPE_ETH,
    .if_ops		= &rtl_8029_netdev_ops,
};

int rtl_8029_probe(struct pcidev_s *pcidev, struct adi_driver_id_s *adi_driver_id)
{
    int i;

    net_device_init(&rtl_8029_netdev);

    pcibus_enable_device(pcidev);

    if (pcidev->dev_resources[0].res_flags & ADI_RES_FLAG_IO)
	rtl_iobase = pcidev->dev_resources[0].res_start;
    else {
	printk(MOD_NAME "device resource 0 is not IO\n");
	return -1;
    }

    printk(MOD_NAME "dev_irq = %d ", pcidev->dev_irq);
    install_irq(pcidev->dev_irq, &rtl_8029_irq_routine);
    printk("iobase = %04x ", rtl_iobase);

    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STP, rtl_iobase + RTL_8029_CR);
    rtl_8029_reset(pcidev);

    outb(0x40 | 0x08 | 0x01, rtl_iobase + RTL_8029_DCR);	/* word-wide 4 Words */
    outb(0x00, rtl_iobase + RTL_8029_IMR);
    outb(0xff, rtl_iobase + RTL_8029_ISR);
    outb(0x20, rtl_iobase + RTL_8029_RCR);    
    outb(0x02, rtl_iobase + RTL_8029_TCR);    
    outb(0x20, rtl_iobase + RTL_8029_RBCR0);
    outb(0x00, rtl_iobase + RTL_8029_RBCR1);
    outb(0x00, rtl_iobase + RTL_8029_RSAR0);
    outb(0x00, rtl_iobase + RTL_8029_RSAR1);
    outb(CR_READ_DMA | CR_STA, rtl_iobase + RTL_8029_CR);
    
    for (i = 0; i < 32; i++) {
	rtl_eprom[i] = inb(rtl_iobase + RTL_8029_DMADATA);
    }

    outb(0x40 | 0x08 | 0x01, rtl_iobase + RTL_8029_DCR);

    outb(CR_PAGE1 | CR_ABORT_DMA | CR_STP, rtl_iobase + RTL_8029_CR);
    
    /* setting mac address in PAR */
    for (i = 0; i < 6; i++) {
	rtl_hwaddr[i] = rtl_eprom[i];    
	rtl_8029_netdev.if_addr[i] = rtl_eprom[i];
	outb(rtl_eprom[i], rtl_iobase + 0x01 + i);
    }
    rtl_8029_netdev.if_addr_len = NET_LAYER_2_ETH_MAC_LEN;

    outb(0x40 + RTL_8029_TX_PAGES, rtl_iobase + RTL_8029_CURR);

    printk("%s hwaddr = %02x %02x %02x %02x %02x %02x\n", rtl_8029_netdev.if_name,
	    rtl_eprom[0], rtl_eprom[1], rtl_eprom[2], rtl_eprom[3], rtl_eprom[4], rtl_eprom[5]);


    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STP, rtl_iobase + RTL_8029_CR);
    outb(0x40 | 0x08 | 0x01, rtl_iobase + RTL_8029_DCR);	/* word-wide 4 Words */
    outb(0x00, rtl_iobase + RTL_8029_RBCR0);
    outb(0x00, rtl_iobase + RTL_8029_RBCR1);
    outb(0x20, rtl_iobase + RTL_8029_RCR);    
    outb(0x02, rtl_iobase + RTL_8029_TCR);   
    outb(0x40, rtl_iobase + RTL_8029_TPSR); 
    outb(0x40 + RTL_8029_TX_PAGES, rtl_iobase + RTL_8029_PSTART);
    outb(0x80, rtl_iobase + RTL_8029_PSTOP);
    outb(0x80 - 1, rtl_iobase + RTL_8029_BNRY);
    outb(0xff, rtl_iobase + RTL_8029_ISR);
    outb(0x00, rtl_iobase + RTL_8029_IMR);

    /*
     * enable packet receive irq, packet transmitted irq
     * receive error irq, transmit error irq, overwrite irq, counter overflow irq
     */   
    outb(IMR_PRXE | IMR_PTXE | IMR_RXEE | IMR_TXEE | IMR_OVWE | IMR_CNTE, rtl_iobase + RTL_8029_IMR);
    outb(0xff, rtl_iobase + RTL_8029_ISR);

    outb(CR_PAGE0 | CR_ABORT_DMA | CR_STA, rtl_iobase + RTL_8029_CR);

    /*
     * normal transmit mode
     */
    outb(0x00, rtl_iobase + RTL_8029_TCR);
    
    /*
     * accept broadcasts
     */
    rtl_8029_set_recv_mode(&rtl_8029_netdev, NET_RECV_MODE_BROADCAST);

    register_net_device(&rtl_8029_netdev);

    return 0;
}

int rtl_8029_remove(struct pcidev_s *pcidev)
{
    return 0;
}

struct adi_driver_pci_s rtl_8029_driver_pci = {
    .adi_driver_probe	= rtl_8029_probe,
    .adi_driver_remove	= rtl_8029_remove
};

struct adi_driver_s rtl_8029_driver = {
    .adi_driver_name	= "rtl_8029 network card",
    .adi_driver_type	= ADI_DRIVER_TYPE_PCI,
    .adi_driver_id	= rtl_8029_driver_id,
    .adi_driver_union.adi_driver_pci = &rtl_8029_driver_pci
};

int adi_netdrv_rtl_8029_init(void)
{
    adi_register_driver(&rtl_8029_driver);

    return 0;
}
