/*
 * kernel_libs/crypt/crypt.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/02
 * Author: 	Lukasz Dembinski
 * Info:	crypt.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */


#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/memory.h>
#include <agnix/crypt.h>
#include <agnix/console.h>

#define MOD_NAME 	"CRYPT: "

struct crypt_algo_s *crypt_algo[MAX_CRYPT_ALGO];

static int crypt_algo_nr = 0;

int get_free_crypt_algo(void)
{
    return crypt_algo_nr++;
}

int put_free_crypt_algo(int cryptd)
{
    return 0;
}

int register_crypt_algo(struct crypt_algo_s *algo)
{
    int crypt_algo_d;
    int ret;

    crypt_algo_d = get_free_crypt_algo();
    
    if (crypt_algo_d < 0)
	return -1;
    
    crypt_algo[crypt_algo_d] = algo;
    printk(MOD_NAME "registering %s crypt algorithm ... ", algo->algo_name);
    
    if (algo->init) {
	ret = algo->init();

	if (ret < 0) {
	    printk("failed\n");
	    return -1;
	}
    }   
    printk("ok\n");
    
    return crypt_algo_d;
}

int unregister_crypt_algo(int cryptd)
{
    put_free_crypt_algo(cryptd);

    return 0;
}

int __init crypt_init(void)
{
    memset(crypt_algo, 0, MAX_CRYPT_ALGO * 4);
    
    DES_init();
/*  MD4_init(); */
/*  MD5_init(); */
    
    return 0;
}
