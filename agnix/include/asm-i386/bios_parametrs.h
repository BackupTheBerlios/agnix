#ifndef _AGNIX_ASM_BIOS_PARAMETRS_H_
#define _AGNIX_ASM_BIOS_PARAMETRS_H_

#define PARAMETRS	((char *)bios_parametrs)

#define E820_MAP_NR	(*(char *)              (PARAMETRS + 0x1e8))
#define E820_MAP	((struct e820_entry_s *)(PARAMETRS + 0x2d0))
#define MEM_SIZE_1	(*(unsigned short *)    (PARAMETRS + 2))
#define MEM_SIZE_2	(*(unsigned long *)     (PARAMETRS + 0x1e0))
#define APM_INFO	((struct apm_info_s *)  (PARAMETRS + 0x40))

extern unsigned long bios_parametrs[1024];

#endif
