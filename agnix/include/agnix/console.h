#ifndef _AGNIX_CONSOLE_H_
#define _AGNIX_CONSOLE_H_

#include <agnix/list.h>
#include <agnix/bugs.h>
#include <agnix/panic.h>

#define NEW_LINE		"\n"

#define MAX_CONSOLE_COLORS	16
#define MAX_CONSOLE_SIZE_Y	25
#define MAX_CONSOLE_SIZE_X	80

struct console_line_s {
    char line_buf[80];
    char line_color[80];
    int  line_offset;
};

struct console_buf_s {
    struct console_line_s cons_lines[MAX_CONSOLE_SIZE_X];
    int cons_line_head;
    int cons_line_tail;
};

struct console_color_s {
    int red;
    int green;
    int blue;
};

struct console_palette_s {
    struct console_color_s colors[MAX_CONSOLE_COLORS];
};

struct console_s {
    struct list_head cons_next;
    int cons_number;
    int cons_real_size_x;
    int cons_real_size_y;
    int cons_virt_size_x;
    int cons_virt_size_y;
    
    struct console_buf_s 	*cons_buf;
    struct console_palette_s 	*cons_palette;
    struct console_ops_s	*cons_ops;

    struct display_s	 	*cons_disp;
    
    char cons_fg_color;
    char cons_bg_color;
};

struct console_ops_s {
    int (*init) (struct console_s *cons);
    int (*reset)(struct console_s *cons);
    int (*puts) (struct console_s *cons, const char *text);
    int (*refresh)(struct console_s *cons, int lines, int line_end);
};

void printk(const char *buf, ...);
//void kernel_panic(const char *text);

#endif
