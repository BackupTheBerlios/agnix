#
# kernel_arch/i386/signals/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = signals.o

akrn_signals.o: $(OBJS)
	$(LD) -r -o akrn_signals.o $(OBJS)

include $(TOP_DIR)/common.make