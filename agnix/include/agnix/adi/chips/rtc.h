#ifndef _AGNIX_CHIPS_RTC_H_
#define _AGNIX_CHIPS_RTC_H_

#include <agnix/adi/adi.h>
#include <agnix/timer.h>

#define RTC_BASE_PORT		0x70
#define RTC_PORT(offset)	RTC_BASE_PORT + offset

#define RTC_READ(address) 	\
({				\
    outb(address, RTC_PORT(0));	\
    inb(RTC_PORT(1));		\
})

#define RTC_WRITE(value, address) 	\
({					\
    outb(address, RTC_PORT(0));		\
    outb(value, RTC_PORT(1));		\
})

#define RTC_REG_SECONDS		0x00
#define RTC_REG_MINUTES		0x02
#define RTC_REG_HOURS		0x04
#define RTC_REG_A		0x0A
#define RTC_REG_B		0x0B
#define RTC_REG_C		0x0C
#define RTC_REG_D		0x0D

#define RTC_UIP			0x80

#define BCD_TO_BIN(val) \
    ((((val >> 4) & 0x0F) * 10) + (val & 0x0F))

#define IS_RTC_UIP		\
    (RTC_READ(RTC_REG_A) & RTC_UIP)


#define rtc_parm(chip_ptr)		((struct chip_rtc_parm_s *)((chip_ptr)->internal_parm))
#define rtc_ops(chip_ptr)		((struct chip_rtc_ops_s *)((chip_ptr)->internal_ops))

struct chip_rtc_ops_s {
    int (*gettime)(struct chip_s *chip, struct timeval_s *tv);    
    int (*settime)(struct chip_s *chip, struct timeval_s *tv);    
};

int rtc_timer_gettimeofday(struct timeval_s *tv);
int rtc_init(void);

#endif

