#ifndef _AGNIX_TASKS_H_
#define _AGNIX_TASKS_H_

#include <agnix/list.h>
#include <agnix/signals.h>
#include <agnix/timer.h>
#include <agnix/timers.h>
#include <asm/types.h>
#include <asm/tss.h>
#include <asm/tasks.h>
#include <asm/regs.h>

#define TASK_STAT_STARTING	0x01
#define	TASK_STAT_RUNNING	0x02
//#define TASK_STAT_STOPPED	0x04
#define TASK_STAT_INIT		0x10
#define TASK_STAT_RESCHED	0x20
#define TASK_STAT_KILLED	0x40

#define INIT_COUNT		10
#define MAX_TASKS		32

#define INIT_TASK(task) \
{						\
    next:	NULL,				\
    prev:	NULL,				\
    run_next:	NULL,				\
    run_prev:	NULL,				\
    count:	INIT_COUNT			\
    state:	TASK_STARTING | TASK_INIT,	\
    flags:	0				\
}    


#ifndef INIT_TASK_SIZE
#define INIT_TASK_SIZE	2048*sizeof(long)
#endif

struct tss_wrap_s {
    struct list_head tss_list;
    u16 tss_entry;
    struct tss_s tss;
    struct task_s *task;
};

struct task_state_s {
    struct tss_regs_s tss_regs;
};

struct task_s {
    struct list_head 		task_list;    
    struct tss_wrap_s 		*tss_wrap;

    char 			*name;

    struct signals_s 		signals;
    struct signals_act_s 	signals_act;
    
    struct timer_s		timer_real;
    struct itimerval_s 		timerval_real;
    u32				timer_real_expire;
    u32				timer_real_interval;
    
    int  			t_pid;
    long 			t_count;
    int  			t_nice;
    int  			t_pri;
    int  			t_usrpri;
    int  			t_uid;
    
    u32   			t_state;
    u32  			t_flags;
    
    struct task_state_s 	state;
};

union task_union {
    struct task_s task;
    unsigned long stack[INIT_TASK_SIZE/sizeof(long)];
    };

struct tss_wrap_s * get_free_tss(void);
void put_free_tss(struct tss_wrap_s *);
void task_switch(u16 nr_task);
u16  task_kernel_create(struct tss_wrap_s *tss_wrap, u32 eip, u32 esp);
int  task_set_user_mode(struct tss_s *tss, u32 eip, u32 esp);
void task_set_handler(struct task_s *task, u32 eip, u32 esp);
void task_restore_state(struct task_s *task, struct task_state_s *state);
void task_save_state(struct task_s *task, struct task_state_s *state);

static inline struct task_s * _current_task(void)
{
    u32 esp;
    
    __asm__ __volatile__ ("movl %%esp, %0"
			    :"=r"(esp));
    
    return (struct task_s *)(((esp - 1) & (~(0x1000 - 1))));
}

#define current_task _current_task()

int fork_add_hash_entry(int pid, struct task_s *task);
struct task_s *fork_find_hash_entry(int pid);

int do_task_exit(struct task_s *task, int exit_code);

#endif
