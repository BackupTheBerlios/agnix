/*
 * kernel_drivers/adi_block/floppy.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/09
 * Author: 	Lukasz Dembinski
 * Info:	floppy.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/ioport.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>

#define BASE_REG		0x3F0

#define FLOPPY_SRA		BASE_REG + 0x00
#define FLOPPY_SRB		BASE_REG + 0x01
#define FLOPPY_DOR		BASE_REG + 0x02
#define FLOPPY_TDR		BASE_REG + 0x03
#define FLOPPY_MSR		BASE_REG + 0x04
#define FLOPPY_DSR		BASE_REG + 0x04
#define FLOPPY_FIFO		BASE_REG + 0x05
#define FLOPPY_RSD		BASE_REG + 0x06
#define FLOPPY_DIR		BASE_REG + 0x07
#define FLOPPY_CCR		BASE_REG + 0x07

/*
 * Floppy SRA register bits
 */
#define SRA_DIR			0x01
#define SRA_WP			0x02
#define SRA_INDX		0x04
#define SRA_HDSEL		0x08
#define SRA_TRK0		0x10
#define SRA_STEP		0x20
#define SRA_DRV2		0x40
#define SRA_INT			0x80

/*
 * Floppy SRB register bits
 */
#define SRB_MOT_EN0		0x01
#define SRB_MOT_EN1		0x02
#define SRB_WE			0x04
#define SRB_RDDATA		0x08
#define SRB_WRDATA		0x10
#define SRB_DRIVE_SEL0		0x20

/*
 * Floppy DOR register bits
 */
#define DOR_DRIVE_SEL0		0x01
#define DOR_DRIVE_SEL1		0x02
#define DOR_RESET_NEG		0x04
#define DOR_DMA_GATE_NEG	0x08
#define DOR_MOT_EN0		0x10
#define DOR_MOT_EN1		0x20
#define DOR_MOT_EN2		0x40
#define DOR_MOT_EN3		0x80

/*
 * Floppy TDR register bits
 */
#define TDR_SEL0		0x01
#define TDR_SEL1		0x02

/*
 * Floppy DSR register bits
 */
#define DSR_DRATE_SEL0		0x01
#define DSR_DRATE_SEL1		0x02
#define DSR_PRE_COMP0		0x04
#define DSR_PRE_COMP1		0x08
#define DSR_PRE_COMP2		0x10
#define DSR_POWER_DOWN		0x40
#define DSR_SW_RESET		0x80

/*
 * Floppy MSR register bits
 */
#define MSR_DRV0_BUSY		0x01
#define MSR_DRV1_BUSY		0x02
#define MSR_DRV2_BUSY		0x04
#define MSR_DRV3_BUSY		0x08
#define MSR_CMD_BUSY		0x10
#define MSR_NON_DMA		0x20
#define MSR_DIO			0x40
#define MSR_RQM			0x80

/*
 * Floppy DIR register bits
 */
#define DIR_HIGH_DENS		0x01
#define DIR_DRATE_SEL0		0x02
#define DIR_DRATE_SEL1		0x04
#define DIR_DSK_CHG		0x80

/*
 * Floppy CCR register bits
 */
#define CCR_DRATE_SEL0		0x01
#define CCR_DRATE_SEL1		0x02

/*
 * Floppy commands
 */
 
#define FLOPPY_CMD_DUMPREGS	0x0E
#define FLOPPY_CMD_VERSION	0x10

#define FLOPPY_MAX_POLL_REQ_REPLIES	0x4000
#define FLOPPY_MAX_POLL_RES_REPLIES	0x4000

#define MOD_NAME		"FLOPPY: "

u8 floppy_DOR = DOR_DMA_GATE_NEG | DOR_RESET_NEG;

struct adi_blkdev_s floppy_dev = {
    .dev_name		= "floppy",
};

int floppy_reset(void)
{
    return 0;
}

int floppy_set_defaults(void)
{
    outb(floppy_DOR, FLOPPY_DOR);

    return 0;
}

int floppy_request_command(u8 command)
{
    int i;
    u8 msr;

    for (i = 0; i < FLOPPY_MAX_POLL_REQ_REPLIES; i++) {
	msr = inb(FLOPPY_MSR);
	if ((msr & MSR_RQM) && (!(msr & MSR_DIO))) {
	    printk("send command ok\n");
	    outb(command, FLOPPY_FIFO);
	    return 0;
	}
    }
    
    return -1;
}

int floppy_result_command(u8 *result_buf, int max_len)
{
    int i;
    int k = 0;
    u8 msr;

    for (k = 0; k < max_len; k++) {
        for (i = 0; i < FLOPPY_MAX_POLL_RES_REPLIES; i++) {
	    msr = inb(FLOPPY_MSR);
	    if ((msr & (MSR_RQM | MSR_DIO | MSR_CMD_BUSY)) == (MSR_RQM | MSR_DIO | MSR_CMD_BUSY)) {
		result_buf[k] = inb(FLOPPY_FIFO);
		break;
	    }
	}
	
	if (i == FLOPPY_MAX_POLL_RES_REPLIES) {
	    printk(MOD_NAME "%d %x\n", k, msr);
	    break;
	}
    }
    
    return k;    
}

int floppy_get_version(void)
{
    u8 result_buf[16];

    adi_blkdev_lock_irq(&floppy_dev);

    //floppy_request_command(FLOPPY_CMD_VERSION);
    floppy_request_command(FLOPPY_CMD_DUMPREGS);
    if (floppy_result_command(result_buf, 16) <= 0) {
	printk(MOD_NAME "bad FDC version\n");
	return -1;
    }
    
    adi_blkdev_unlock_irq(&floppy_dev);
    
    printk(MOD_NAME "FDC version = %02x\n", result_buf[0]);
    
    return -1;
}

int floppy_init(void)
{
    floppy_set_defaults();
    floppy_get_version();

    return 0;
}

int adi_blkdrv_floppy_init(void)
{
    return floppy_init();
}
