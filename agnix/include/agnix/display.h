#ifndef _AGNIX_DISPLAY_H_
#define _AGNIX_DISPLAY_H_

#define COLOR_BLUE	1
#define COLOR_GREEN	2
#define COLOR_RED	3
#define COLOR_SILVER	7
#define COLOR_WHITE	15
#define COLOR_BLACK	0

struct display_adapter_s {
};

struct display_s;

struct display_ops_s {
    int (*init)(void);
    int (*putc)(struct display_s *disp, char c, int pos_x, int pos_y, char color);
};

struct display_s {
    void  *disp_base_addr;
    struct display_ops_s *ops;
    int disp_orientation;
    int disp_size_x;
    int disp_size_y;
};

#endif
