#ifndef _AGNIX_MEMORY_H_
#define _AGNIX_MEMORY_H_

#include <agnix/memory/memory.h>
#include <asm/memory.h>

void *memset(void *from, int c, int count);
void *memcpy(void *from, void *to, int count);
int memcmp(void *what, void *with, int count);

#endif
