#ifndef _AGNIX_ASM_FASTRAND_H_
#define _AGNIX_ASM_FASTRAND_H_

#include <asm/types.h>

unsigned int _fastrand(u32 value);
unsigned int fastrand(u32 range);

#endif
