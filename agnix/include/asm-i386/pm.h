#ifndef __AGNIX_ASM_PM_H__
#define __AGNIX_ASM_PM_H__

#include <asm/types.h>

struct pm_ops_s {
    u8 (*pm_get_power_state)(void);
    void (*pm_set_power_state)(void);
    void (*pm_save_power_state)(void);
    void (*pm_restore_power_state)(void);
};

#endif
