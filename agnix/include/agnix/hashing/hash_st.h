#ifndef _AGNIX_HASHING_HASH_ST_H_
#define _AGNIX_HASHING_HASH_ST_H_

#include <asm/types.h>

int hash_entry_add(int hashd, u32 hash_key, u32 hash_addr);
u32 hash_entry_find(int hashd, u32 hash_key);
int register_hash_table(u32 rows, u32 (*hash_fn)(u32 key));
int usregister_hash_table(int hashd);

#endif
