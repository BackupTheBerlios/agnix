#
# kernel_arch/i386/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

#OBJS = fastrand.o faults.o head.o init_task.o ioport.o irq.o machine.o pm.o setup.o syscalls.o task.o timer.o
#OBJS_1 = fastrand.o faults.o ioport.o irq.o machine.o pm.o setup.o syscalls.o task.o timer.o
#OBJS_2 = bios/akrn_bios.o cpu/akrn_cpu.o memory/akrn_memory.o signals/akrn_signals.o

OBJ-y = fastrand.o faults.o head.o init_task.o ioport.o irq.o machine.o pm.o setup.o syscalls.o task.o timer.o
OBJ-l = fastrand.o faults.o ioport.o irq.o machine.o pm.o setup.o syscalls.o task.o timer.o
OBJ_SUBDIR = bios/akrn_bios.o cpu/akrn_cpu.o memory/akrn_memory.o signals/akrn_signals.o
OBJ_LINK = $(OBJ-l) $(OBJ_SUBDIR)
OBJ_TARGET = krn_arch.o
SUBDIR-y := bios cpu memory signals

#krn_arch.o: $(OBJS)
#	$(MAKE) -C bios
#	$(MAKE) -C cpu
#	$(MAKE) -C memory
#	$(MAKE) -C signals
#	$(LD) -r -o krn_arch.o $(OBJS_1) $(OBJS_2)

include $(TOP_DIR)/common.make
