#include <agnix/agnix.h>
#include <agnix/tasks.h>
#include <agnix/threads.h>
#include <agnix/signals.h>
#include <agnix/memory.h>
#include <agnix/console.h>
#include <agnix/unistd.h>

#define MOD_NAME	"SIG: "

/* fix it \/ \/ \/ \/ \/ \/ bleah */
int execute_signal_sigaction(struct regs_s *regs, struct sigaction_s *sigaction, int sig_nr)
//int execute_signal_sigaction(struct task_s *task, struct sigaction_s *sigaction, int sig_nr)
{
//    struct tss_s *tss = &(task->tss_wrap->tss);
/*    u32 task_esp;
    u32 signal_esp;
    u32 signal_eip;
    struct signals_frame_s *frame;

    if (sigaction->sa_handler == (void *)0 || sigaction->sa_handler == (void *)1)
	return -1;

    task_esp = regs->esp;
    signal_esp = (task_esp - 16) - sizeof(struct signals_frame_s);
    signal_eip = (u32)sigaction->sa_handler;

    frame = (struct signals_frame_s *)(task_esp - 16);
    frame->retcode_ptr = (u32)(frame->retcode);
    frame->sig_nr = sig_nr;
    frame->retcode[0] = 0xb8;
    frame->retcode[1] = 0x58;
    frame->retcode[2] = __NR_sigreturn;
    frame->retcode[3] = 0x80;
    frame->retcode[4] = 0xcd;
*/
//    task_save_state(task, &(task->state));
//    task_set_handler(task, signal_eip, signal_esp);

    return 0;
}

int execute_signals(struct task_s *task, struct regs_s *regs)
{
    struct signal_s *signal;
    void (* sa_handler)(int);
    int sig_nr;
    
    signal = signal_dequeue(task);
    sig_nr = signal->signal_nr;

    printk(MOD_NAME "dequeued signal nr %d %x\n", signal->signal_nr, regs->eip);

    switch(signal->signal_nr) {

	case SIGSTOP:
	    task->t_state |= TASK_STAT_STOPPED;
	    break;	

	case SIGKILL:
	    task->t_state |= TASK_STAT_KILLED;
	    do_task_exit(task, 0);
	    break;	
	
	default:
	    if (USER_THREAD(task)) {
		execute_signal_sigaction(regs, &(task->signals_act.sigaction[sig_nr]), sig_nr);
	    }
	    else {
		sa_handler = task->signals_act.sigaction[sig_nr].sa_handler;

	    	if (sa_handler)
		    sa_handler(sig_nr);
	    }
    }

    put_free_pages((u32)signal, 0);    

    return 0;
}
