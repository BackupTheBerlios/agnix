#ifndef _AGNIX_SCHED_H_
#define _AGNIX_SCHED_H_

#include <agnix/tasks.h>
#include <agnix/sleep.h>

void schedule_task(void);
void idle_state_enter(void);
void sched_activate_task(struct task_s *task);
void sched_activate_task_lock(struct task_s *task);
void sched_deactivate_task(struct task_s *task);
void sched_deactivate_task_lock(struct task_s *task);

#endif
