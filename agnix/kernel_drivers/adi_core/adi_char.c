/*
 * kernel_drivers/adi_core/adi_char.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_char.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>

#define MOD_NAME	"CHAR: "

extern int adi_chrdrv_kbd_init(void);
extern int adi_sysconsole_init(void);
extern int adi_text_display_init(void);

int __init adi_console_init(void)
{
    adi_text_display_init();
    adi_sysconsole_init();

    return 0;
}

int __init adi_chrdrv_init(void)
{
    printk(MOD_NAME "Initializing character drivers\n");

    adi_chrdrv_kbd_init();
    
    return 0;
}
