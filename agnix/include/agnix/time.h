#ifndef _AGNIX_TIME_H_
#define _AGNIX_TIME_H_

struct tms_s {
    clock_t tms_utime;
    clock_t tms_stime;
    clock_t tms_cutime;
    clock_t tms_cstime;
};

#endif
