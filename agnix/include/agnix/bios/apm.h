#ifndef _AGNIX_APM_H_
#define _AGNIX_APM_H_

#include <asm/types.h>

#define APM_CPU_IDLE		0x5305
#define APM_CPU_BUSY		0x5306
#define APM_SET_POWER_STATE	0x5307
#define APM_ENABLE_APM		0x5308
#define APM_RESTORE_DEF_APM	0x5309
#define APM_GET_POWER_STATUS	0x530A
#define APM_GET_PM_EVENT	0x530B
#define APM_GET_POWER_STATE	0x530C
#define APM_ENABLE_DEV_APM	0x530D
#define APM_DRIVER_VERSION	0x530E
#define APM_ENGAGE_APM		0x530F

#define APM_BIOS		0x0000
#define APM_ALL_DEVICES		0x0001

#define APM_POWER_STATE_READY	0x0000
#define APM_POWER_STATE_STANDBY 0x0001
#define APM_POWER_STATE_SUSPEND	0x0002
#define APM_POWER_STATE_OFF	0x0003

#define APM_DISABLE		0x0000
#define APM_ENABLE		0x0001

#define APM_DEFAULT		0x0000

/*
 * APM event codes
 */
#define APM_EVENT_STANDBY		0x0001
#define APM_EVENT_SUSPEND		0x0002
#define APM_EVENT_NORMAL_RESUME		0x0003
#define APM_EVENT_CRITICAL_RESUME	0x0004
#define APM_EVENT_LOW_BATTERY		0x0005
#define APM_EVENT_POWER_STATUS_CHANGE	0x0006
#define APM_EVENT_UPDATE_TIME		0x0007
#define APM_EVENT_CRITICAL_SUSPEND	0x0008
#define APM_EVENT_USER_STANDBY		0x0009
#define APM_EVENT_USER_SUSPEND		0x000A
#define APM_EVENT_STANDBY_RESUME	0x000B
#define APM_EVENT_CAPABILITY_CHANGE	0x000C

struct apm_info_s {
    u16 apm_version;
    u16 apm_cseg;
    u32 apm_offset;
    u16 apm_cseg_16;
    u16 apm_dseg;
    u16 apm_flags;
    u16 apm_cseg_len;
    u16 apm_cseg_16_len;
    u16 apm_dseg_len;
};

struct apm_power_status_s {
    u8  apm_battery_status;
    u8  apm_battery_flag;
    u8  apm_battery_life_perc;
    u16 apm_battery_life_time;
};

int apm_set_power_state(u8 dest, u8 state);
int apm_cpu_idle(void);
int apm_cpu_busy(void);

#endif
