#
# kernel_libs/crypt/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#


OBJS = crypt.o DES.o

lkrn_crypt.o: $(OBJS)
	$(LD) -r -o lkrn_crypt.o $(OBJS)

include $(TOP_DIR)/common.make