#
# kernel_net/layer_2/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJ_SUBDIR-$(CONFIG_NET_LAYER_2_ETH) 	:= eth/eth.o
OBJ_LINK := $(OBJ_SUBDIR-y)
OBJ_TARGET := layer_2.o

SUBDIR-$(CONFIG_NET_LAYER_2_ETH)	:= eth

#all:
#	$(MAKE) -C pci
#	$(LD) -r -o adi_buses_drv.o $(OBJS)

include $(TOP_DIR)/common.make