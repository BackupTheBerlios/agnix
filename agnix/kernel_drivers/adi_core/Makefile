#
# kernel_drivers/adi_core/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJ-y = adi_buses.o 
OBJ-$(CONFIG_DRIVERS_CHAR)  += adi_char.o
OBJ-$(CONFIG_DRIVERS_BLOCK) += adi_block.o
OBJ-y += adi_chips.o 
OBJ-y += adi_core.o 
OBJ-y += adi_drivers.o 
OBJ-y += adi_devices.o 
OBJ-$(CONFIG_DRIVERS_NET) += adi_net.o
OBJ-y += adi_resources.o

OBJ_LINK = $(OBJ-y)
OBJ_TARGET = adi_core_drv.o

#adi_core_drv.o: $(OBJS)
#	$(LD) -r -o adi_core_drv.o $(OBJS)

include $(TOP_DIR)/common.make
