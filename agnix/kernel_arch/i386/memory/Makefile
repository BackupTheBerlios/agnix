#
# kernel_arch/i386/memory/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

#OBJS = memory.o pgmap.o
#OBJS_2 = memtests/akrn_memtests.o

OBJ-y 				:= memory.o pgmap.o
OBJ_SUBDIR-$(CONFIG_MEMTESTS) 	:= memtests/akrn_memtests.o
OBJ_LINK 			:= $(OBJ-y) $(OBJ_SUBDIR-y)
OBJ_TARGET 			= akrn_memory.o

SUBDIR-$(CONFIG_MEMTESTS)	= memtests

#akrn_memory.o: $(OBJS)
#	$(MAKE) -C memtests
#	$(LD) -r -o akrn_memory.o $(OBJS) $(OBJS_2)

include $(TOP_DIR)/common.make