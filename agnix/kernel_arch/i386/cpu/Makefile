#
# kernel_arch/i386/cpu/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = bogo.o cpu.o cpuid.o gates.o msr.o mtrr.o pmc.o setup.o tsc.o

akrn_cpu.o: $(OBJS)
	$(LD) -r -o akrn_cpu.o $(OBJS)

include $(TOP_DIR)/common.make