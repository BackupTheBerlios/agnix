#
# kernel_drivers/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

#include $(TOP_DIR)/kernel_config/.config

#OBJ_SUBDIR-y := adi_buses/adi_buses_drv.o adi_char/adi_chr_drv.o adi_chips/adi_chips_drv.o adi_core/adi_core_drv.o adi_net/adi_net_drv.o
OBJ_SUBDIR-y 				:= adi_buses/adi_buses_drv.o
OBJ_SUBDIR-y 				+= adi_chips/adi_chips_drv.o
OBJ_SUBDIR-y				+= adi_core/adi_core_drv.o
OBJ_SUBDIR-$(CONFIG_DRIVERS_CHAR) 	+= adi_char/adi_chr_drv.o
OBJ_SUBDIR-$(CONFIG_DRIVERS_NET) 	+= adi_net/adi_net_drv.o
OBJ_LINK = $(OBJ-y) $(OBJ_SUBDIR-y)

SUBDIR-y 			:= adi_buses
SUBDIR-y 			+= adi_chips
SUBDIR-y 			+= adi_core
SUBDIR-$(CONFIG_DRIVERS_CHAR)	+= adi_char
SUBDIR-$(CONFIG_DRIVERS_NET)	+= adi_net

OBJ_TARGET = krn_drivers.o

#all:
#	$(MAKE) -C adi_buses
#	$(MAKE) -C adi_char
#	$(MAKE) -C adi_chips
#	$(MAKE) -C adi_core
#	$(MAKE) -C adi_net
#	$(LD) -r -o krn_drivers.o $(OBJS-y)

include $(TOP_DIR)/common.make
