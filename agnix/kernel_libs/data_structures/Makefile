#
# kernel_libs/data_structures/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJS = data_str.o btree.o hash_st.o hash_dn.o

lkrn_datastr.o: $(OBJS)
	$(LD) -r -o lkrn_datastr.o $(OBJS)

include $(TOP_DIR)/common.make