#
# kernel_net/Makefile
#
# Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
# All Rights Reserved
# 
# Date:		2004/07
# Author: 	Lukasz Dembinski
# Info:		Makefile
# Contact:	mailto: <dembol@linux.net>
#

OBJ_SUBDIR-y			:= layer_2/layer_2.o
#OBJ_SUBDIR-y 			+= layer_3/layer_3.o
#OBJ_SUBDIR-y			+= layer_4/layer_4.o
#OBJ_SUBDIR-y			+= sockets/sockets.o
OBJ_SUBDIR-y			+= net_core/core.o
OBJ_LINK = $(OBJ_SUBDIR-y)

SUBDIR-y 			:= layer_2
#SUBDIR-y 			+= layer_3
#SUBDIR-y 			+= layer_4
#SUBDIR-y			+= sockets
SUBDIR-y 			+= net_core

OBJ_TARGET = krn_net.o

include $(TOP_DIR)/common.make
