/*
 * kernel_arch/i386/kernel/memory/pgmap.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	pgmap.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/memory.h>
#include <asm/paging.h>
#include <agnix/console.h>

#define MOD_NAME		"MEM: "

extern struct main_memory_s main_memory;

struct pte_s *pte_ptr(struct pde_s *pde)
{
    struct pte_s *pte;
    
    if (!pte_address(pde)) {
	pte = (struct pte_s *)get_free_pages(0);
	pde->pte_addr = virt_to_phys((u32)pte);
	set_pde(pde, make_pde_phys(pde->pte_addr, PDE_FLAG_P | PDE_FLAG_RW));
    } else {
    //	set_pde(pde, make_pde_phys(pde->pte_addr, PDE_FLAG_P | PDE_FLAG_RW));
	pte = phys_to_virt(pte_address(pde));
    }
    
    return pte;
}

int remap_page_tables(u32 pd_addr, u32 from, u32 to, u32 len)
{
    

    return 0;    
}

int remap_kpage_tables(u32 from, u32 to, u32 len)
{
    return remap_page_tables(main_memory.pdbr, from, to, len);
}

int __init page_tables_init(struct main_memory_s *mem)
{
    struct pde_s *pde;
    struct pte_s *pte;
    u32    page_start;
    u32    page_end;
    u32    page_len;
    u32    page_count;
    int    i, j;

#ifdef DEBUG_MEM    
    printk(MOD_NAME "Initializing page tables\n");
#endif
    
    page_count = 0;
    page_start = 0;
    page_end   = (mem->mem_size) >> 2; /* KB >> 2 == pages */
    page_len = page_end - page_start;

    pde = (struct pde_s *)mem->pdbr;
    pde += pde_offset(PAGE_OFFSET);

#ifdef DEBUG_MEM
    printk(MOD_NAME "count  = %d\n", page_len);
    printk(MOD_NAME "offset = %d\n", pde_offset(PAGE_OFFSET));
#endif

    for (i = pde_offset(PAGE_OFFSET); i < PDE_PER_PD; i++) {

	pte = pte_ptr(pde);
	for (j = 0; j < PTE_PER_PT; j++) {

	    set_pte(pte, make_pte_phys(page_count << PAGE_SHIFT, (PTE_FLAG_P | PTE_FLAG_RW)));	    	
//	    printk("%08x\n", page_count << PAGE_SHIFT);

	    if (++page_count >= page_len)
		break;

	    pte++;
	}
	
	pde++;

	if (page_count >= page_len)
	    break;
    }

#ifdef DEBUG_MEM
    printk(MOD_NAME "Loading cr3 (phys_addr=%x) ... ", virt_to_phys(mem->pdbr));
#endif

    load_cr3(mem->pdbr);
    
#ifdef DEBUG_MEM
    printk("ok\n");
#endif

    return 0;
}
