#ifndef _AGNIX_MATH64_H_
#define _AGNIX_MATH64_H_

#include <asm/types.h>

void add_64_64(u32 *value, u32 *op1, u32 *op2);
void add_64_32(u32 *value, u32 *op1, u32 op2);
void sub_64_64(u32 *value, u32 *op1, u32 *op2);
void sub_64_32(u32 *value, u32 *op1, u32 op2);
void mul_32_32(u32 *value, u32 op1, u32 op2);
void div_64_32(u32 *value, u32 *op1, u32 op2);

#endif
