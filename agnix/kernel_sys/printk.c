/*
 * kernel_sys/printk.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	printk.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h> 
#include <agnix/init.h> 

#if CONFIG_DRIVERS_CHAR

#include <agnix/memory.h>
#include <agnix/terminal.h>
#include <agnix/display.h>
#include <stdarg.h>
#include <asm/irq.h>

extern struct terminal_s term_sys[MAX_TERMINALS];
extern int vsnprintf(char *buf, int size, const char *fmt, va_list args);
extern int term_cur;
extern int term_init;

void printk(const char *text, ...)
{
    va_list args;
    char text_buf[1024];
    int len;
    u32 flags;
    int i;

    if (!term_init)
	return;

    save_flags(flags); __cli();
    
    memset(text_buf, 0, 1024);

    va_start(args, text);
    len = vsnprintf(text_buf, 1024, text, args);
    va_end(args);

    /* printk to all terminals */
    for (i = 0; i < MAX_TERMINALS; i++) {
	if (term_sys[i].ops && term_sys[i].ops->write)
    	    term_sys[i].ops->write(&term_sys[i], text_buf);
    }
    
    restore_flags(flags);
}

#else

void printk(const char *text, ...)
{
}

#endif
