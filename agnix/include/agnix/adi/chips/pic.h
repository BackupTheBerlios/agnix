#ifndef _AGNIX_CHIPS_PIC_H_
#define _AGNIX_CHIPS_PIC_H_

#include <agnix/adi/adi.h>
//chips.h>
#include <asm/types.h>

#define INT_CNTRL_1		0x20
#define INT_CNTRL_2		0xA0
#define PIC_MASK_BYTES		0x08

#define pic_parm(chip_ptr)		((struct chip_pic_parm_s *)((chip_ptr)->internal_parm))
#define pic_ops(chip_ptr)		((struct chip_pic_ops_s *)((chip_ptr)->internal_ops))

struct chip_pic_ops_s {
    int (*startup_irq)(struct chip_s *chip, u8 irq);
    int (*shutdown_irq)(struct chip_s *chip, u8 irq);
    int (*enable_irq)(struct chip_s *chip, u8 irq);
    int (*disable_irq)(struct chip_s *chip, u8 irq);
    void (*ack_irq)(struct chip_s *chip, u8 irq);
    void (*mask_irq)(struct chip_s *chip, u8 irq);
};

struct chip_pic_parm_s {
    u32 irq_mask[PIC_MASK_BYTES];
    u32 irq_count[256];
};

int i8259a_init(void);

#endif

