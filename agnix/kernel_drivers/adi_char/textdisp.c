/*
 * kernel_drivers/adi_char/textdisp.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	textdisp.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/init.h>
#include <agnix/display.h>
#include <agnix/ioport.h>

#define TEXT_DISP_BASE_ADDR	0xb8000

struct display_s text_display;
struct display_ops_s text_display_ops;

int textdisp_putc(struct display_s *disp, char c, int pos_x, int pos_y, char color)
{
    int size_x = disp->disp_size_x;
    char *addr = (char *)(disp->disp_base_addr) + ((pos_y * size_x + pos_x) << 1);

    *addr = c;
    *(addr + 1) = color;    

    return 0;    
}

int textdisp_init(void) 
{
    int i;

    for (i = 0; i < 16; i++) {
	inb(0x3da);
	outb(i, 0x3c0);
	outb(i, 0x3c0);
    }
    outb(0x20, 0x3c0);
    
    return 0;
}

int __init adi_text_display_init(void)
{
    text_display_ops.init = textdisp_init;
    text_display_ops.putc = textdisp_putc;

    text_display.disp_base_addr = (char *)TEXT_DISP_BASE_ADDR;
    text_display.disp_size_x = 80;
    text_display.disp_size_y = 25;
    text_display.ops = &text_display_ops;

    textdisp_init();
    
    return 0;
}

