#ifndef _AGNIX_CHIPS_PIT_H_
#define _AGNIX_CHIPS_PIT_H_

#include <agnix/adi/adi.h>
#include <asm/types.h>

#define SYS_TIMER_FREQ		1000
#define i8254_FREQ		1193180
#define i8254_RELOAD_COUNT	(i8254_FREQ / SYS_TIMER_FREQ)	

#define TMRCNT		0x40
#define TCW		0x43
#define TMRSTS		TMRCNT

#define pit_parm(chip_ptr)		((struct chip_pit_parm_s *)((chip_ptr)->internal_parm))
#define pit_ops(chip_ptr)		((struct chip_pit_ops_s *)((chip_ptr)->internal_ops))

struct chip_pit_ops_s {
    void (*set_mode)(struct chip_s *chip, u8 counter, u8 mode, u8 bcd);
    void (*write_count)(struct chip_s *chip, u8 counter, u16 count);
    u16  (*read_count)(struct chip_s *chip, u8 counter);
};

struct chip_pit_parm_s {
};

int i8254_init(void);

#endif

