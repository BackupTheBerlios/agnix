#ifndef _AGNIX_TIMER_H_
#define _AGNIX_TIMER_H_

#define TIMER_HZ	1000
#define jiffies 	(jiffies_64[0])

struct timeval_s {
    int tv_sec;
    int tv_usec;
};

struct itimerval_s {
    struct timeval_s it_interval;
    struct timeval_s it_value;
};

int timer_gettimeofday(struct timeval_s *tv);
int timer_settimeofday(struct timeval_s *tv);

#endif
