/*
 * kernel_libs/data_structures/data_str.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	data_str.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/console.h>

#define MOD_NAME	"DATA_STR: "

extern int hash_static_init(void);
extern int btree_init(void);

int data_structures_init(void)
{
    if (hash_static_init() < 0)
	kernel_panic(MOD_NAME "Static hashing init error!");

    if (btree_init() < 0)
	kernel_panic(MOD_NAME "Btree init error!");

    return 0;
}
