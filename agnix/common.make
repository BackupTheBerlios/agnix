
include $(TOP_DIR)/kernel_config/.config

subdirs = $(sort $(patsubst %,_agnix_subdir_%, $(SUBDIR-y)))

objs = $(sort $(patsubst %,_agnix_obj_%, $(OBJ-y)))
objs_all: objs_ok subdirs_ok

subdirs_ok: $(subdirs)
	$(LD) -r -o $(OBJ_TARGET) $(OBJ_LINK)

objs_ok: $(objs)

$(objs) :
	$(MAKE) $(patsubst _agnix_obj_%,%,$@)

$(subdirs) : 
	$(MAKE) -C $(patsubst _agnix_subdir_%,%,$@)




.c.s:
	$(CC) $(CFLAGS) -S -o $*.s $<
.s.o:
	$(AS) $(AFLAGS) -c -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<
%.o: %.s
	$(AS) $(AFLAGS) $(EXTRA_CFLAGS) -o $@ $<
%.s: %.S
	$(CPP) $(AFLAGS) $(EXTRA_AFLAGS) $(AFLAGS_$@) $< > $@
%.o: %.S
	$(CC) $(AFLAGS) -c -o $@ $<

	