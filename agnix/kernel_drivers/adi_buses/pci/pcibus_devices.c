#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/adi/adi.h>
#include <agnix/console.h>

#define MOD_NAME "PCI: \t"

int adi_pcibus_device_init(struct adi_driver_pci_s *adi_driver_pci, struct adi_driver_id_s *adi_driver_id)
{
    struct pcidev_s *pcidev;
    int idx = 0;

    do {
	if (adi_driver_id[idx].device_id == 0)
	    return -1;
	
	if ((pcidev = pcibus_find_device_by_id(adi_driver_id[idx].vendor_id, adi_driver_id[idx].device_id))) {
	    if (adi_driver_pci->adi_driver_probe) {
		if (adi_driver_pci->adi_driver_probe(pcidev, &adi_driver_id[idx]) < 0)
		    printk(MOD_NAME "PCI driver probe failed for %s\n", pcidev->dev_name);
		else
		    printk(MOD_NAME "PCI driver probe ok for %s\n", pcidev->dev_name);
	    }
	}
	
	idx++;
    } while (adi_driver_id[idx].device_id);    

    return 0;
}
