
all: bbootsect bsetup tools/build
	tools/build -b bbootsect bsetup $(TOP_DIR)/vmagnix $(ROOT_DEV) > bzImage

tools/build: tools/build.c
	$(CC) -o tools/build tools/build.c
#	$(HOSTCC) $(HOSTCFLAGS) -o $@ $< -I$(TOPDIR)/include

bootsect: bootsect.o
	$(LD) -Ttext 0x0 -s --oformat binary -o $@ $<

bootsect.o: bootsect.s
	$(AS) -o $@ $<

bootsect.s: bootsect.S Makefile $(BOOT_INCL)
	$(CPP) $(CPPFLAGS) -traditional $(SVGA_MODE) $(RAMDISK) $< -o $@

bbootsect: bbootsect.o
	$(LD) -Ttext 0x0 -s --oformat binary $< -o $@

bbootsect.o: bbootsect.s
	$(AS) -o $@ $<

bbootsect.s: bootsect.S Makefile $(BOOT_INCL)
	$(CPP) $(CPPFLAGS) -D__BIG_KERNEL__ -traditional $(SVGA_MODE) $(RAMDISK) $< -o $@

setup: setup.o
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $@ $<

setup.o: setup.s
	$(AS) -o $@ $<

setup.s: setup.S video.S Makefile
	$(CPP) $(CPPFLAGS) -D__ASSEMBLY__ -traditional $(SVGA_MODE) $(RAMDISK) $< -o $@

# $(BOOT_INCL) $(TOPDIR)/include/agnix/version.h $(TOPDIR)/include/agnix/compile.h

bsetup: bsetup.o
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $@ $<

bsetup.o: bsetup.s
	$(AS) -o $@ $<

bsetup.s: setup.S video.S Makefile
	$(CPP) $(CPPFLAGS) -D__BIG_KERNEL__ -D__ASSEMBLY__ -traditional $(SVGA_MODE) $(RAMDISK) $< -o $@


# $(BOOT_INCL) $(TOPDIR)/include/agnix/version.h $(TOPDIR)/include/agnix/compile.h
