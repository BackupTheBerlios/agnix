#ifndef _AGNIX_ADI_PCIBUS_NAMES_H_
#define _AGNIX_ADI_PCIBUS_NAMES_H_

struct pcibus_vendor_name_s {
    u32 vendor_id;
    const char *vendor_name;
};

struct pcibus_device_name_s {
    u32 vendor_id;
    u32 device_id;
    const char *device_name;
};

const char *pcibus_vendor_name(u32 vendor_id);
const char *pcibus_device_name(u32 vendor_id, u32 device_id);

#endif
