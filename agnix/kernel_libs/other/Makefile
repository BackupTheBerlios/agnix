#
# kernel_libs/other/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = memory.o vsprintf.o strings.o

lkrn_other.o: $(OBJS)
	$(LD) -r -o lkrn_other.o $(OBJS)

include $(TOP_DIR)/common.make