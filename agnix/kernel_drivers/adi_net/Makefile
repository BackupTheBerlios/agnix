#
# kernel_drivers/adi_net/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJ-$(CONFIG_DRIVERS_NET_3c59x) = 3c59x.o
OBJ-$(CONFIG_DRIVERS_NET_RTL_8029) = rtl_8029.o
OBJ_LINK = $(OBJ-y)
OBJ_TARGET = adi_net_drv.o

#adi_net_drv.o: $(OBJS)
#	$(LD) -r -o adi_net_drv.o $(OBJS)

include $(TOP_DIR)/common.make
