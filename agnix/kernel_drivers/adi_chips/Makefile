#
# kernel_drivers/adi_chips/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = i387.o i8254.o i8259a.o rtc.o

adi_chips_drv.o: $(OBJS)
	$(LD) -r -o adi_chips_drv.o $(OBJS)

include $(TOP_DIR)/common.make