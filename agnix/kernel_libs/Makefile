#
# kernel_libs/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

SUBDIR-y = crypt data_structures math other queues

OBJ_SUBDIR = crypt/lkrn_crypt.o data_structures/lkrn_datastr.o math/lkrn_math.o \
other/lkrn_other.o queues/lkrn_queues.o
OBJ_LINK = $(OBJ-y) $(OBJ_SUBDIR)
OBJ_TARGET = krn_libs.o


#krn_libs.o:
#	$(MAKE) -C crypt
#	$(MAKE) -C data_structures
#	$(MAKE) -C math
#	$(MAKE) -C other
#	$(MAKE) -C queues
#	$(LD) -r -o krn_libs.o $(OBJS)

include $(TOP_DIR)/common.make
