/*
 * kernel_drivers/adi_core/adi_chips.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_chips.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/init.h>
#include <agnix/adi/adi.h>
#include <agnix/irq.h>
#include <agnix/console.h>

#define MOD_NAME		"CHIPS: "

const char *chip_class_names[] = {
    "CPU", "PIC", "PIT", "RTC",
};

int adi_register_chip(struct chip_s *chip)
{
    int ret = 0;

    if (chip->chip_name)
        printk(MOD_NAME "Registering %s chip\t (class %s)\n", chip->chip_name, chip_class_names[chip->chip_class]);
    else
	printk(MOD_NAME "Registering unknown-name chip\n", chip->chip_name);

    spin_lock_init(&chip->chip_lock);

    /* + request resources */

    chip->chip_state = CHIP_STATE_INITIALIZING;

    if ((chip->chip_ops) && (chip->chip_ops->init)) {
	
	ret = chip->chip_ops->init(chip);
	if (ret)
	    return -1;
	else
	    chip->chip_state = CHIP_STATE_WORKING;
    }

    return 0;
}

void __init adi_chips_init(void)
{
    printk(MOD_NAME "Initializing chip drivers\n");

    i8259a_init();
     i8254_init();
       rtc_init();
     intel_init(0);
}
