#ifndef _AGNIX_ASM_SIGNALS_H_
#define _AGNIX_ASM_SIGNALS_H_

#include <agnix/types.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <asm/regs.h>

/* it is defined in agnix/tasks.h */
struct task_s;

int execute_signals(struct task_s *task, struct regs_s *regs);

#endif
