#
# kernel_drivers/adi_buses/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = pci/adi_pcibus_drv.o

all:
	$(MAKE) -C pci
	$(LD) -r -o adi_buses_drv.o $(OBJS)

include $(TOP_DIR)/common.make