#
# kernel_arch/i386/bios/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = apm.o bios.o smbios.o

akrn_bios.o: $(OBJS)
	$(LD) -r -o akrn_bios.o $(OBJS)

include $(TOP_DIR)/common.make