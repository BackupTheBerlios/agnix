#
# kernel_drivers/adi_char/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = keyboard.o syscons.o terminal.o textdisp.o

adi_chr_drv.o: $(OBJS)
	$(LD) -r -o adi_chr_drv.o $(OBJS)

include $(TOP_DIR)/common.make