/*
 * kernel_sys/time.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/09
 * Author: 	Lukasz Dembinski
 * Info:	time.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/timer.h>

int do_sys_time(int *secs)
{
    struct timeval_s tv;

    timer_gettimeofday(&tv);
    *secs = tv.tv_sec;
    
    return tv.tv_sec;
}

int do_sys_stime(int *secs)
{
    struct timeval_s tv;

    tv.tv_sec = *secs;
    timer_settimeofday(&tv);
    
    return 0;
}

