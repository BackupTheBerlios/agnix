/*
 * kernel_drivers/adi_char/syscons.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	syscons.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/memory.h>
#include <agnix/terminal.h>
#include <agnix/display.h>
#include <agnix/panic.h>

#define SCREEN_SCROLL_LINES		10

extern struct display_s text_display;
extern int kbd_screen_scroll;

int term_cur;
int term_init = 0;
struct terminal_s term_sys[MAX_TERMINALS];
struct terminal_driver_s term_sys_driver;

int syscons_count_offset(struct terminal_s *term, char *buf, int screen_scroll)
{
    int lines  = 0;
    int head   = term->term_buf.head;
    int size_x = term->term_var->term_size_x;
    int size_y = term->term_var->term_size_y;
    int term_data_len = term->term_buf.data_len;
    int i;
    int j;

    i = head;
    j = 0;

    do {
        if (i == 0) {
	    return 0;
//	    i = term_data_len;
	}
	else if (i > 0)
	    i--;

    	if ((buf[i] == '\n') || (j == size_x - 1)) {
	    j = 0;
	    lines++;
	}
	else if (buf[i] == 0)
	    break;
	else
	    j++;
	
    } while(lines != size_y - 1 + screen_scroll * SCREEN_SCROLL_LINES);
    
//    if (lines != size_y - 1 + screen_scroll * SCREEN_SCROLL_LINES) {
//	if (screen_scroll > 0)
//	    screen_scroll--;
//    }

    return (i + 1) % term_data_len;
}

int syscons_refresh(struct terminal_s *term, int screen_scroll)
{
    struct display_s *disp = term->term_driver->display;
    char *term_data = term->term_buf.data;
    int term_data_len = term->term_buf.data_len;
    int size_x = term->term_var->term_size_x;
    int size_y = term->term_var->term_size_y;
    int i;
    int x, y;
    
    i = syscons_count_offset(term, term_data, screen_scroll);

    for (y = 0; y < size_y; y++) {
	for (x = 0; x < size_x; x++) {
	    if ((term_data[i] != '\n') && (term_data[i] != 0)) {
	        disp->ops->putc(disp, term_data[i], x, y, 7);
		i = (i + 1) % term_data_len;
	    }
	    else
		disp->ops->putc(disp, ' ', x, y, 7);
	}
	
	if (term_data[i] == '\n')
	    i = (i + 1) % term_data_len;
    }
    
    return 0;
}

int syscons_init(struct terminal_s *term)
{
    return 0;
}

int syscons_write_char(struct terminal_s *term, char character)
{
    int head 		= term->term_buf.head;
    char *term_data 	= term->term_buf.data;
    int term_data_len 	= term->term_buf.data_len;

    if (term->term_buf.data == NULL) {
        term->term_buf.data = (char *)get_free_pages(1);
	if (term->term_buf.data == NULL)
	    return -1;
	    
        head          = term->term_buf.head = term->term_buf.tail = 0;
        term_data_len = term->term_buf.data_len = (2 << PAGE_SHIFT) - 1;
        term_data     = term->term_buf.data;
    }
	
    term_data[head] 	= character;
    head 		= (head + 1) % term_data_len;
    term->term_buf.head = head;

    if (term->term_active) {
        if (term->term_driver && term->term_driver->ops->refresh) {
	    kbd_screen_scroll = 0;
	    term->term_driver->ops->refresh(term, 0);
	}
    }
    
    return 0;
}

int syscons_write(struct terminal_s *term, char *buf)
{
    int head 		= term->term_buf.head;
    char *term_data 	= term->term_buf.data;
    int term_data_len 	= term->term_buf.data_len;
    int i 		= 0;

    if (buf[0] == 0)
	return 0;

    do {	

	if (term->term_buf.data == NULL) {
	    term->term_buf.data = (char *)get_free_pages(1);
	    if (term->term_buf.data == NULL)
		return -1;

	    head          = term->term_buf.head = term->term_buf.tail = 0;
	    term_data_len = term->term_buf.data_len = (2 << PAGE_SHIFT) - 1;
	    term_data     = term->term_buf.data;
	}
	
	term_data[head] = buf[i];
        head = (head + 1) % term_data_len;
	
	i++;
    } while(buf[i] != 0);

    term->term_buf.head = head;
    term_data[head]	= 0;

    if (term->term_active) {
        if (term->term_driver && term->term_driver->ops->refresh) {
	    kbd_screen_scroll = 0;
	    term->term_driver->ops->refresh(term, 0);
	}
    }

    return i;
}

struct terminal_driver_ops_s term_sys_driver_ops = { 
    .refresh		= syscons_refresh
};

struct terminal_var_s term_sys_var = { 
    .term_size_x	= 80,
    .term_size_y	= 25 
};

struct terminal_ops_s term_sys_ops = { 
    .init 		= syscons_init,
    .write 		= syscons_write,
    .write_char		= syscons_write_char
};

struct terminal_driver_s term_sys_driver = {
    .ops		= &term_sys_driver_ops,
    .display		= &text_display
};

int __init adi_sysconsole_init(void)
{
    int i;

//    memset(&term_sys_driver, 0, sizeof(term_sys_driver));
//    term_sys_driver.ops = &term_sys_driver_ops;
//    term_sys_driver.display = &text_display;

    for (i = 0; i < MAX_TERMINALS; i++) {
	memset(&term_sys[i], 0, sizeof(struct terminal_s));

	INIT_LIST_HEAD(&(term_sys[i].term_list));
	INIT_LIST_HEAD(&(term_sys[i].term_sys_list));
	INIT_LIST_HEAD(&(term_sys[i].wait_queue));

	term_sys[i].term_var 	= &term_sys_var;
	term_sys[i].ops 	= &term_sys_ops;
        term_sys[i].term_active = 0;
	
	if (register_terminal(&term_sys[i]) < 0)
	    kernel_panic("register_terminal() error\n");       
	
	register_terminal_driver(&term_sys[i], &term_sys_driver);
    }

    term_sys[0].term_active = 1;
    term_cur		    = 0;
    term_init		    = 1;
    
    return 0;
}
