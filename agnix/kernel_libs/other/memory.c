/*
 * kernel_libs/other/memory.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	memory.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/console.h>

void *memset(void *from, int c, int n) 
{
    int i;
    
    for (i = 0; i < n; i++)
    {
	*(char *)from = (char)c;
	from++;
    }

    return from;
}

void *memcpy(void *to, void *from, int n) 
{
    int i;
    
    for (i = 0; i < n; i++)
    {
	*(char *)to = *(char *)from;
	from++;
	to++;
    }

    return from;
}

int memcmp(void *what, void *with, int n)
{
//    int r = 0;
    int i;

    for (i = 0; i < n; i++) {

//	printk("%d with %d\n", *(char *)what, *(char *)with);

	if (*(char *)what != *(char *)with)
	    return -1;

	what++;
	with++;
    }

    return 0;
}
