#ifndef _AGNIX_TERMINAL_H_
#define _AGNIX_TERMINAL_H_

#include <agnix/list.h>

#define MAX_TERMINALS		16
//#define TERMINALS_BITMAP_LEN	(((MAX_TERMINALS - 1) >> 5) + 1)

struct terminal_s;

struct terminal_buf_s {
    int head;
    int tail;
    int data_len;
    char *data;
};

struct terminal_ops_s {
    int (* init)(struct terminal_s *term);
    int (* read)(struct terminal_s *term, char *pack);    
    int (* write)(struct terminal_s *term, char *pack);    
    int (* write_char)(struct terminal_s *term, char character);    
    int (* reset)(struct terminal_s *term);
};

struct terminal_var_s {
    int term_size_x;
    int term_size_y;
};

struct terminal_cmap_s {
    int term_cur_fgc;
    int term_cur_bgc;
};

struct terminal_driver_ops_s {
    int (* init)(void);
    int (* refresh)(struct terminal_s *term, int screen_scroll);
};

struct terminal_driver_s {
    struct list_head term_driver_list;
    struct terminal_driver_ops_s *ops;
    struct display_s *display;
};

struct terminal_s {
    const char *term_name;
    int term_nr;
    struct list_head term_list;
    struct list_head term_sys_list;
//    struct list_head recv_packs;
//    struct list_head send_packs;
    struct terminal_buf_s term_buf;
    struct list_head wait_queue;
    struct terminal_ops_s *ops;
    struct terminal_var_s *term_var;
    struct terminal_driver_s *term_driver;
    
    int term_active;
    int term_pgrp;
        
//    int max_remem_packs;
};

int terminal_switch(int term_new);
int register_terminal(struct terminal_s *term);
int unregister_terminal(int term_nr);
int register_terminal_driver(struct terminal_s *term, struct terminal_driver_s *term_driver);
int terminals_init(void);

#endif

