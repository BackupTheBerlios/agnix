
#include <agnix/agnix.h>
#include <agnix/init.h>

int apm_init(void);

int __init pm_init(void)
{
#if CONFIG_BIOS_APM
    apm_init();
#endif

    return 0;
}
