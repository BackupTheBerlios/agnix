/*
 * kernel_drivers/adi_chips/i8259a.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	i8259a.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/spinlock.h>
#include <asm/types.h>
#include <agnix/ioport.h>
#include <agnix/memory.h>
#include <agnix/irq.h>
#include <agnix/console.h>

#define I8259A_CASCADE_IRQ	2

struct chip_pic_parm_s 	i8259a_pic_parm;

int i8259a_startup_irq(struct chip_s *chip, u8 irq)
{
    return 0;
}

int i8259a_shutdown_irq(struct chip_s *chip, u8 irq)
{
    return 0;
}

int i8259a_enable_irq(struct chip_s *chip, u8 irq)
{
    CHIP_LOCK;

    pic_parm(chip)->irq_mask[0] &= ~(1 << irq);
    outb((pic_parm(chip)->irq_mask[0]) & 0xFF, INT_CNTRL_1 + 1);
    outb((pic_parm(chip)->irq_mask[0] >> 8) & 0xFF, INT_CNTRL_2 + 1);

    CHIP_UNLOCK;

    return 0;
}

int i8259a_disable_irq(struct chip_s *chip, u8 irq)
{
    CHIP_LOCK;

    pic_parm(chip)->irq_mask[0] |= 1 << irq;
    outb((pic_parm(chip)->irq_mask[0]) & 0xFF, INT_CNTRL_1 + 1);
    outb((pic_parm(chip)->irq_mask[0] >> 8) & 0xFF, INT_CNTRL_2 + 1);

    CHIP_UNLOCK;


    return 0;
}

void i8259a_ack_irq(struct chip_s *chip, u8 irq)
{
    CHIP_LOCK;

    if (irq <= 7)
	outb(0x60 + (irq & 0x07), INT_CNTRL_1);
    else
    if (irq <= 15) {
	outb(0x60 + (irq & 0x07), INT_CNTRL_2);
    	outb(0x60 + I8259A_CASCADE_IRQ, INT_CNTRL_1);
    }

    CHIP_UNLOCK;
}

void i8259a_mask_irq(struct chip_s *chip, u8 irq)
{
    CHIP_LOCK;
    CHIP_UNLOCK;
}

struct chip_pic_ops_s 	i8259a_pic_ops = 
{
    .startup_irq 	= i8259a_startup_irq,
    .shutdown_irq	= i8259a_shutdown_irq,
    .enable_irq		= i8259a_enable_irq,
    .disable_irq	= i8259a_disable_irq,
    .ack_irq		= i8259a_ack_irq,
    .mask_irq		= i8259a_mask_irq
};

void i8259a_cascade_irq(u32 data)
{
    printk("cascade irq\n");
}

struct irq_routine_s i8259a_cascade_irq_routine = {
    .proc		= i8259a_cascade_irq,
};

int i8259a_chip_cascade_init(struct chip_s *chip)
{
    install_irq(I8259A_CASCADE_IRQ, &i8259a_cascade_irq_routine);
    
    return 0;
}

int i8259a_chip_init(struct chip_s *chip)
{
    CHIP_LOCK;

    memset(pic_parm(chip)->irq_mask, 0xFF, PIC_MASK_BYTES);

    outb(0x11, INT_CNTRL_1);
    outb(0x20, INT_CNTRL_1 + 1);
    outb(0x04, INT_CNTRL_1 + 1);
    outb(0x01, INT_CNTRL_1 + 1);
    
    outb(0x11, INT_CNTRL_2);
    outb(0x28, INT_CNTRL_2 + 1);
    outb(0x02, INT_CNTRL_2 + 1);
    outb(0x01, INT_CNTRL_2 + 1);
    
    outb((u8)((pic_parm(chip)->irq_mask[0]) & 0xFF), INT_CNTRL_1 + 1);
    outb((u8)((pic_parm(chip)->irq_mask[0] >> 8) & 0xFF), INT_CNTRL_2 + 1);

    CHIP_UNLOCK;

    i8259a_chip_cascade_init(chip);

    return 0;
}

int i8259a_chip_release(struct chip_s *chip)
{
    return 0;
}

struct chip_ops_s 	i8259a_chip_ops = {
    .init		= i8259a_chip_init,
    .release		= i8259a_chip_release
};

struct chip_s	  	i8259a_chip = {
    .chip_name 		= "i8259a",
    .chip_class		= CHIP_CLASS_PIC,
    .chip_vendor	= 0x8086,
    .chip_priority	= 90,
    .chip_ops		= &i8259a_chip_ops,
    .chip_pm_ops	= NULL,
    .internal_ops 	= &i8259a_pic_ops,
    .internal_parm 	= &i8259a_pic_parm
};


int __init i8259a_init(void)
{
    adi_register_chip(&i8259a_chip);

    return 0;    
}
