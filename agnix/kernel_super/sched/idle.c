#include <agnix/agnix.h>
#include <agnix/tasks.h>
#include <agnix/bios/bios.h>
#include <agnix/irq.h>

#if CONFIG_BIOS_APM
#define CPU_IDLE_LOOPS	10
#endif

void idle_state_enter(void)
{

#if CONFIG_BIOS_APM
    int i;
    u32 flags;
    
    save_flags(flags); __cli();
    
    for (i = 0; i < CPU_IDLE_LOOPS; i++)
	apm_cpu_idle();

    apm_cpu_busy();
    
    restore_flags(flags);
    
#else

    while(!(current_task->t_state & TASK_STAT_RESCHED))
	__asm__ __volatile__ ("hlt");
    
#endif    
}
