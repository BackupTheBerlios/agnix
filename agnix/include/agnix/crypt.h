#ifndef _AGNIX_CRYPT_H_
#define _AGNIX_CRYPT_H_

#define MAX_CRYPT_ALGO		16
#define MAX_CRYPT_NAME_LEN	32

struct crypt_algo_s {
    char *algo_name;
    int (*init)(void);
    int (*crypt)(char *in_buf, char *out_buf, char *passwd, int size);
    int (*decrypt)(char *in_buf, char *out_buf, char *passwd, int size);
};

int register_crypt_algo(struct crypt_algo_s *algo);
int unregister_crypt_algo(int cryptd);

int crypt_init(void);

#include <agnix/crypt/des.h>

#endif
