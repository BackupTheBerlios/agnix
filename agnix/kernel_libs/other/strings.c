/*
 * kernel_libs/other/strings.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	strings.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#define STRINGS_MAX_LEN		4096

int strlen(char *buf)
{
    int i;

    if (buf[0] == 0)
	return 0;

    for(i = 0; i < STRINGS_MAX_LEN; i++) {
	if (buf[i] == 0)
	    return i - 1;
    }
    
    return -1;
}
