#ifndef _AGNIX_ADI_CHIPS_H_
#define _AGNIX_ADI_CHIPS_H_

#include <agnix/spinlock.h>
#include <agnix/list.h>
#include <asm/types.h>
#include <asm/pm.h>

#define CHIP_STATE_INITIALIZING		0x01
#define CHIP_STATE_WORKING		0x02

#define CHIP_LOCK  \
    u32 flags;	\
    spin_lock_irqsave(&chip->chip_lock, flags);

#define CHIP_UNLOCK  \
    spin_unlock_irqrestore(&chip->chip_lock, flags); 

struct chip_ops_s;

struct chip_s {
    char 		*chip_name;
    u32	 		chip_class;
    u32	 		chip_vendor;
    u32			chip_priority;

    struct chip_ops_s 	*chip_ops;
    struct pm_ops_s	*chip_pm_ops;

    void 		*internal_ops;
    void 		*internal_parm;    

    u32			chip_capability;
    u32			chip_state;
    struct list_head	chip_list;
    spinlock_t		chip_lock;
};

struct chip_ops_s {
    int (*init)(struct chip_s *chip);
    int (*release)(struct chip_s *chip);
};

#include <agnix/adi/chips/class.h>
#include <agnix/adi/chips/cpu.h>
#include <agnix/adi/chips/pic.h>
#include <agnix/adi/chips/pit.h>
#include <agnix/adi/chips/rtc.h>

int adi_register_chip(struct chip_s *chip);
void adi_chips_init(void);

#endif
