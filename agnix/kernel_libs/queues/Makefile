#
# kernel_libs/queues/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = queues.o fifo.o lifo.o

lkrn_queues.o: $(OBJS)
	$(LD) -r -o lkrn_queues.o $(OBJS)

include $(TOP_DIR)/common.make