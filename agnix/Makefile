VERSION =    0
PATCHLEVEL = 0
SUBLEVEL =   3
EXTRAVERSION = -unstable

AS:=as
CC:=gcc
MAKE:=make
NM:=nm

KERNELRELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

ARCH := $(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/)

TOP_DIR:= $(shell /bin/pwd)
CORE_FILES:= kernel_sys/kernel.o
OUTPUT_FILE:=/dev/null

CFLAGS:= -D__KERNEL -I/FreeSpace/src/agnix-sf/include  -I/usr/lib/gcc-lib/i386-slackware-linux/2.95.3/include \
-Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fno-builtin -fomit-frame-pointer \
-mpreferred-stack-boundary=2 -nostdinc\

AFLAGS:=-D__ASSEMBLY__ -D__KERNEL__ -I/FreeSpace/src/agnix-sf/include -traditional

CPPFLAGS:=-D__KERNEL__ -I$(TOP_DIR)/include

FLAGS:= $(CFLAGS) -c -o

LINKFLAGS:= -T $(TOP_DIR)/kernel_boot/$(ARCH)/vmagnix.ld -e stext

HEAD_FILES:=kernel_arch/$(ARCH)/head.o kernel_arch/$(ARCH)/init_task.o
LINK_FILES:=kernel_sys/krn_sys.o kernel_super/krn_super.o kernel_libs/krn_libs.o kernel_drivers/krn_drivers.o kernel_arch/$(ARCH)/krn_arch.o kernel_net/krn_net.o
CLEAN_FILES:=$(TOP_DIR)/vmagnix
OUTPUT_FILE:=/dev/null
BASH=/bin/bash
MAKEFILES=$(TOP_DIR)/kernel_config/.config

export AFLAGS CFLAGS CPPFLAGS FLAGS TOP_DIR AS CC MAKE BASH MAKEFILES

include kernel_config/.config

clean:
	find . -name '*.[aos]' | xargs rm -f
	rm -f $(CLEAN_FILES)

vmagnix.o: newversion include/agnix/version.h
	$(LD) $(LINKFLAGS) $(HEAD_FILES) $(LINK_FILES)	\
		--start-group \
		--end-group -e startup_32 \
		-Ttext 0xc0100000 \
		-o vmagnix.o
	$(NM) vmagnix.o | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > System.map
	cat System.map | sort > System_sort.map

vmagnix: include/agnix/version.h vmagnix.o
	$(LD) $(LINKFLAGS) $(HEAD_FILES) $(LINK_FILES)	\
		--start-group \
		--end-group -e startup_32 \
		--oformat binary -Ttext 0xc0100000 \
		-o vmagnix


symlinks:
	rm -f include/asm
	( cd include ; ln -sf asm-$(ARCH) asm)
	@if [ ! -d include/agnix/modules ]; then \
		mkdir include/agnix/modules; \
	fi

menuconfig: include/agnix/version.h symlinks
	$(MAKE) -C scripts/lxdialog all
	/bin/bash scripts/Menuconfig kernel_config/config.in


$(TOPDIR)/include/agnix/version.h: include/agnix/version.h
$(TOPDIR)/include/agnix/compile.h: include/agnix/compile.h

newversion:
	. scripts/mkversion > .tmpversion
	@mv -f .tmpversion kernel_config/.version

uts_len		:= 64
uts_truncate	:= sed -e 's/\(.\{1,$(uts_len)\}\).*/\1/'

include/agnix/compile.h: include/agnix/version.h
	@echo -n \#`cat kernel_config/.version` > .ver1
	@if [ -n "$(CONFIG_SMP)" ] ; then echo -n " SMP" >> .ver1; fi
	@if [ -f .name ]; then  echo -n \-`cat .name` >> .ver1; fi
	@LANG=C echo ' '`date` >> .ver1
	@echo \#define UTS_VERSION \"`cat .ver1 | $(uts_truncate)`\" > .ver
	@LANG=C echo \#define AGNIX_COMPILE_TIME \"`date +%T`\" >> .ver
	@echo \#define AGNIX_COMPILE_BY \"`whoami`\" >> .ver
	@echo \#define AGNIX_COMPILE_HOST \"`hostname | $(uts_truncate)`\" >> .ver
	@([ -x /bin/dnsdomainname ] && /bin/dnsdomainname > .ver1) || \
	 ([ -x /bin/domainname ] && /bin/domainname > .ver1) || \
	 echo > .ver1
	@echo \#define AGNIX_COMPILE_DOMAIN \"`cat .ver1 | $(uts_truncate)`\" >> .ver
	@echo \#define AGNIX_COMPILE_PLATFORM \"`uname`\" >> .ver
	@echo \#define AGNIX_COMPILER \"`$(CC) $(CFLAGS) -v 2>&1 | tail -1`\" >> .ver
	@mv -f .ver include/agnix/compile.h
	@rm -f .ver1

include/agnix/version.h: ./Makefile
	@cat kernel_config/.version
	@expr length "$(KERNELRELEASE)" \<= $(uts_len) > /dev/null || \
	  (echo KERNELRELEASE \"$(KERNELRELEASE)\" exceeds $(uts_len) characters >&2; false)
	@echo \#define UTS_RELEASE \"$(KERNELRELEASE)\" > .ver
	@echo \#define AGNIX_VERSION_CODE `expr $(VERSION) \\* 65536 + $(PATCHLEVEL) \\* 256 + $(SUBLEVEL)` >> .ver
	@echo '#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))' >>.ver
	@echo '#define AGNIX_LICENSE "GNU General Public License"' >>.ver
	@mv -f .ver $@

bzImage: include/agnix/version.h include/agnix/compile.h
	$(MAKE) -C kernel_drivers
	$(MAKE) -C kernel_libs
	$(MAKE) -C kernel_super
	$(MAKE) -C kernel_arch/i386
	$(MAKE) -C kernel_sys
	$(MAKE) -C kernel_net
	$(MAKE) vmagnix
	$(MAKE) -C kernel_boot/i386/boot

include $(TOP_DIR)/common.make


