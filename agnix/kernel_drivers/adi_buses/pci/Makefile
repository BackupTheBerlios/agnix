#
# kernel_drivers/adi_buses/pci/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#


OBJS = pcibus_core.o pcibus_device_names.o pcibus_irq.o pcibus_names.o pcibus_ops.o pcibus_scan.o pcibus_vendor_names.o pcibus_devices.o

all: pcibus_names
	make adi_pcibus_drv.o

pcibus_names: pcibus_names_gen
	rm -f pcibus_define_names.h
	rm -f pcibus_device_names.c
	rm -f pcibus_vendor_names.c
	./pcibus_names_gen
	cp pcibus_define_names.h $(TOP_DIR)/include/agnix/adi/buses/adi_pcibus_define_names.h

pcibus_names_gen: pcibus_names_gen.c
	gcc -o pcibus_names_gen pcibus_names_gen.c

adi_pcibus_drv.o: $(OBJS)
	$(LD) -r -o adi_pcibus_drv.o $(OBJS)


include $(TOP_DIR)/common.make