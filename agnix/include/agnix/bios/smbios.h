#ifndef _AGNIX_BIOS_SMBIOS_H_
#define _AGNIX_BIOS_SMBIOS_H_

#include <agnix/agnix.h> /* no needed */

#define _SM_	"_SM_"
#define _DMI_	"_DMI_"

struct smbios_eps_s {
    char ident[4];
    u8  eps_checksum;
    u8  eps_length;
    u8  smbios_major;
    u8  smbios_minor;
    u16 smbios_str_max_size;
    u8  ep_rev;
    u8  formatted_area[5];
};

struct smbios_ieps_s {
    char ident[5];
    u8  ieps_checksum;
    u16 smbios_str_len;
    u32 smbios_str_addr;
    u16 smbios_str_nr;
    u8  smbios_bcd_rev;
};

int smbios_init(void);

#endif
