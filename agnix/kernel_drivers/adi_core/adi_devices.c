/*
 * kernel_drivers/adi_core/adi_devices.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	adi_devices.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/adi/adi.h>

void adi_device_lock_irq(struct adi_device_lock_s *dev_lock)
{
    spin_lock_irqsave(&dev_lock->spin_lock, dev_lock->save_flags);
}

void adi_device_unlock_irq(struct adi_device_lock_s *dev_lock)
{
    spin_unlock_irqrestore(&dev_lock->spin_lock, dev_lock->save_flags);
}
