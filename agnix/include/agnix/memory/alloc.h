#ifndef _AGNIX_MEMORY_ALLOC_H_
#define _AGNIX_MEMORY_ALLOC_H_

#include <asm/types.h>
#include <agnix/list.h>

struct allocator_s {
    u32 mem_start;		/* start page */
    u32 mem_end;		/* end page */
    u32 mem_size;
    u32 mem_size_pages;
    u32 mem_table_size_pages;
};

#endif
