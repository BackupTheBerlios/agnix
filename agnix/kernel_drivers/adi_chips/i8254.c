/*
 * kernel_drivers/adi_chips/i8254.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	i8254.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */
#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/spinlock.h>
#include <asm/types.h>
#include <agnix/ioport.h>
#include <agnix/memory.h>

u16 i8254_read_count(struct chip_s *chip, u8 counter)
{
    u8 latch_word = 0;
    u8 lsb, msb;
    u16 count;

    CHIP_LOCK;

    latch_word |= (counter << 6);
    
    outb(latch_word, TCW);
    lsb = inb(TMRCNT + counter);
    msb = inb(TMRCNT + counter);
    count = (msb << 8) | lsb;

    CHIP_UNLOCK;

    return count;
}

void i8254_write_count(struct chip_s *chip, u8 counter, u16 count)
{
    CHIP_LOCK;

    outb((count) & 0xFF, TMRCNT + counter);
    outb((count >> 8) & 0xFF, TMRCNT + counter);
    
    CHIP_UNLOCK;
}

void i8254_set_mode(struct chip_s *chip, u8 counter, u8 mode, u8 bcd)
{
    u8  tcw_word = 0;

    CHIP_LOCK;

    tcw_word |= (counter << 6) | (3 << 4) | (mode << 1) | bcd;
    outb(tcw_word, TCW);

    CHIP_UNLOCK;
}

struct chip_pit_ops_s 	i8254_pit_ops = 
{
    i8254_set_mode,
    i8254_write_count,
    i8254_read_count    
};

int i8254_chip_init(struct chip_s *chip)
{
    CHIP_LOCK;

    /* disable speaker, enable gate */
    outb((inb(0x61) & ~0x02) | 0x01, 0x61);
    
    /* counter 0, mode 2, binary */
    i8254_set_mode(chip, 0, 2, 0);
    i8254_write_count(chip, 0, i8254_RELOAD_COUNT);
    
    CHIP_UNLOCK;
        
    return 0;
}

int i8254_chip_release(struct chip_s *chip)
{
    return 0;
}

struct chip_ops_s 	i8254_chip_ops = {
    .init		= i8254_chip_init,
    .release		= i8254_chip_release
};

struct chip_s	  	i8254_chip = {
    .chip_name 		= "i8254",
    .chip_class		= CHIP_CLASS_PIT,
    .chip_vendor	= 0x8086,
    .chip_priority	= 90,
    .chip_ops		= &i8254_chip_ops,
    .chip_pm_ops	= NULL,
    .internal_ops 	= &i8254_pit_ops,
};

int __init i8254_init(void)
{
    adi_register_chip(&i8254_chip);

    return 0;    
}
