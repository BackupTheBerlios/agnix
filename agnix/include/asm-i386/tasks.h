#ifndef _AGNIX_ASM_TASKS_H_
#define _AGNIX_ASM_TASKS_H_

#include <asm/regs.h>

struct tss_wrap_s *task_fork(struct regs_s *regs, u32 esp);
int task_exit(struct tss_wrap_s *tss_wrap);

#endif
