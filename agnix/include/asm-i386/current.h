#ifndef _I386_CURRENT_H
#define _I386_CURRENT_H

#include <agnix/tasks.h>

static inline struct task_s * get_current(void)
{
	struct task_s *current;
	__asm__("andl %%esp,%0; ":"=r" (current) : "0" (~8191UL));
	return current;
 }
 
#define current get_current()

#endif /* !(_I386_CURRENT_H) */
