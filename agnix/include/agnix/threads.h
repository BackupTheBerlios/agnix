#ifndef _AGNIX_THREAD_H_
#define _AGNIX_THREAD_H_

#include <agnix/agnix.h>
#include <agnix/linkage.h>

#define KERNEL_THREAD(x) (!(x->tss_wrap->tss.cs & 0x03))
#define USER_THREAD(x)   (x->tss_wrap->tss.cs & 0x03)

asmlinkage int create_kernel_thread(const char *name, void (*thread_proc)(void *data), void *data);
asmlinkage int create_user_thread  (const char *name, void (*thread_proc)(void *data), void *data);

#endif
