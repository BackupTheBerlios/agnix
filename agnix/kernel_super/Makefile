#
# kernel_super/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#



SUBDIR-y := init
SUBDIR-y += memory
SUBDIR-y += sched
SUBDIR-y += signals
SUBDIR-y += timers
SUBDIR-y += counters

OBJ_LINK = init/skrn_init.o memory/skrn_memory.o sched/skrn_sched.o signals/skrn_signals.o timers/skrn_timers.o counters/skrn_counters.o
OBJ_TARGET = krn_super.o

OBJ-y =

#all:
#	$(MAKE) -C init
#	$(MAKE) -C memory
#	$(MAKE) -C sched
#	$(MAKE) -C signals
#	$(MAKE) -C timers
#	$(LD) -r -o krn_super.o $(OBJS)

include $(TOP_DIR)/common.make
