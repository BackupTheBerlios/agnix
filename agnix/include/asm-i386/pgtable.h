#ifndef _AGNIX_ASM_PGTABLE_H_
#define _AGNIX_ASM_PGTABLE_H_

#include <asm/irq.h>	/* __save_flags(), restore_flags(), __cli() */

#define PAGE_OFFSET	0xc0000000

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)

#define virt_to_phys(addr)	(addr - PAGE_OFFSET)
#define phys_to_virt(addr)	(addr + PAGE_OFFSET)
#define virt_to_page(addr)	(virt_to_phys(addr) >> PAGE_SHIFT)

#ifndef __ASSEMBLY__

#include <asm/types.h>

#define PDE_PER_PD	1024
#define PTE_PER_PT	1024

#define PDE_ADDR_MASK	0xFFFFF000
#define PDE_FLAG_P	0x001
#define PDE_FLAG_RW	0x002
#define PDE_FLAG_US	0x004
#define PDE_FLAG_PWT	0x008
#define PDE_FLAG_PCD	0x010
#define PDE_FLAG_A	0x020
#define PDE_FLAG_PS	0x080
#define PDE_FLAG_G	0x100

#define PTE_ADDR_MASK	PDE_ADDR_MASK
#define PTE_FLAG_P	PDE_FLAG_P
#define PTE_FLAG_RW	PDE_FLAG_RW
#define PTE_FLAG_US	PDE_FLAG_US
#define PTE_FLAG_PWT	PDE_FLAG_PWT
#define PTE_FLAG_PCD	PDE_FLAG_PCD
#define PTE_FLAG_A	PDE_FLAG_A
#define PTE_FLAG_D	0x040
#define PTE_FLAG_PAT	0x080
#define PTE_FLAG_G	PDE_FLAG_G

#define DIR_ADDR_SHIFT		22
#define DIR_ADDR_MASK		0x3FF

#define TABLE_ADDR_SHIFT	12
#define TABLE_ADDR_MASK		0x3FF

struct pde_s {
    u32 flags		: 9;
    u8  avail		: 3;
    u32 pte_addr	: 20;
};

struct pte_s {
    u32 flags		: 9;
    u8  avail		: 3;
    u32 pg_addr		: 20;
};

#define pde_v(x) 		(struct pde_s)(x)
#define pte_v(x) 		(struct pte_s)(x)
#define pde_p(x) 		(struct pde_s *)(x)
#define pte_p(x) 		(struct pte_s *)(x)

#define make_pde(linear_addr, flags)	((virt_to_phys(linear_addr) & PDE_ADDR_MASK) + flags & (~PDE_ADDR_MASK))
#define make_pde_phys(phys_addr, flags)	((phys_addr & PDE_ADDR_MASK) | ((flags) & (~PDE_ADDR_MASK)))
#define make_pte(linear_addr, flags)	((virt_to_phys(linear_addr) & PTE_ADDR_MASK) + flags & (~PTE_ADDR_MASK))
#define make_pte_phys(phys_addr, flags)	((phys_addr & PTE_ADDR_MASK) | ((flags) & (~PTE_ADDR_MASK)))

static inline void set_pde(struct pde_s *pde, u32 pde_val) {
    *(u32 *)pde = pde_val;
}

static inline void set_pte(struct pte_s *pte, u32 pte_val) {
    *(u32 *)pte = pte_val;
}

static inline u32 pde_offset(u32 address) {
    return ((address >> DIR_ADDR_SHIFT) & DIR_ADDR_MASK);
}

static inline u32 pte_offset(u32 address) {
    return ((address >> TABLE_ADDR_SHIFT) & TABLE_ADDR_MASK);
}

static inline void load_cr3(u32 pdbr)
{
    u32 flags;
    
    __save_flags(flags); __cli();

    __asm__ __volatile__ ("movl %%eax, %%cr3"
			  :
			  :"a"(virt_to_phys(pdbr))
			  );
			  
    __restore_flags(flags);
}

#define pte_address(pde) pte_p((pde)->pte_addr)
#define pg_address(pte)  (u32)((pte)->pg_addr)

#endif

#endif
