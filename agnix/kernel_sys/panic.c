/*
 * kernel_sys/panic.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	panic.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/console.h>

void forever_loop(void)
{
    for(;;);
}

void kernel_panic(const char *text)
{
    printk(text);

    forever_loop();
}
