/*
 * kernel_drivers/adi_chips/rtc.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	rtc.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>
#include <agnix/ioport.h>
#include <agnix/memory.h>
#include <agnix/timer.h>

int rtc_timer_gettimeofday(struct timeval_s *tv)
{
    int i;
    
    /* must be a spin_lock! */

    for(i = 0; i < 1000000; i++)
	if (IS_RTC_UIP) break;

    for(i = 0; i < 1000000; i++)
	if (!IS_RTC_UIP) break;

    tv->tv_sec  = BCD_TO_BIN(RTC_READ(RTC_REG_SECONDS))
	        + BCD_TO_BIN(RTC_READ(RTC_REG_MINUTES)) * 60
		+ BCD_TO_BIN(RTC_READ(RTC_REG_HOURS)) * 60 * 60;
		
    tv->tv_usec = 0;

    return 0;
}

int rtc_gettime(struct chip_s *chip, struct timeval_s *tv)
{
    int ret;

    CHIP_LOCK;

    ret = rtc_timer_gettimeofday(tv);

    CHIP_UNLOCK;
    
    return ret;
}

int rtc_settime(struct chip_s *chip, struct timeval_s *tv)
{
    return 0;
}

struct chip_rtc_ops_s 	rtc_int_ops = 
{
    rtc_gettime,
    rtc_settime
};

int rtc_chip_init(struct chip_s *chip)
{
    CHIP_LOCK;
    CHIP_UNLOCK;

    return 0;
}

int rtc_chip_release(struct chip_s *chip)
{
    CHIP_LOCK;
    CHIP_UNLOCK;

    return 0;
}

struct chip_ops_s 	rtc_chip_ops = {
    .init		= rtc_chip_init,
    .release		= rtc_chip_release
};

struct chip_s	  	rtc_chip = {
    .chip_name 		= "rtc",
    .chip_class		= CHIP_CLASS_RTC,
    .chip_vendor	= 0x8086,
    .chip_priority	= 90,
    .chip_ops		= &rtc_chip_ops,
    .chip_pm_ops	= NULL,
    .internal_ops 	= (void *)&rtc_int_ops,
    .internal_parm 	= (void *)0
};

int __init rtc_init(void)
{
    adi_register_chip(&rtc_chip);

    return 0;    
}
