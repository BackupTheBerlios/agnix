#ifndef _AGNIX_SIGNALS_H_
#define _AGNIX_SIGNALS_H_

#include <agnix/types.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
//#include <agnix/tasks.h>

#define NR_SIG_WORDS	2
#define WORD_SIZE	32
#define NSIG		NR_SIG_WORDS * WORD_SIZE

/* it is defined in agnix/tasks.h */
struct task_s;

typedef struct {
	u32 sig[NR_SIG_WORDS];
} sigset_t;

//#define NSIG		32
#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
#define SIGLOST		29
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31
#define SIGRTMIN	32
#define SIGRTMAX	(NSIG-1)

#define MAX_SIGNAL_QUEUES	NSIG
#define MAX_SIGNAL_ACTIONS	NSIG

struct signals_frame_s {
    u32 retcode_ptr;
    int sig_nr;
    u8  retcode[0x10];
};

/* be compatibily with Linux */
struct sigaction_s {
    void (*sa_handler)(int val);
    unsigned long sa_flags;
    void (*sa_restorer)(void);
    sigset_t sa_mask;
};

struct signals_act_s {
    struct sigaction_s sigaction[MAX_SIGNAL_ACTIONS];
    spinlock_t lock;
};

struct signals_s {
    u32 pending;
    sigset_t  sigset;
    sigset_t  sigmask;
    spinlock_t lock;
    
    struct list_head list[MAX_SIGNAL_QUEUES];
};

/* follow structure was raw copied from linux's asm/siginfo.h */

#define SI_MAX_SIZE	128
#define SI_PAD_SIZE	((SI_MAX_SIZE/sizeof(int)) - 3)

typedef union sigval {
	int sival_int;
	void *sival_ptr;
} sigval_t;

typedef struct siginfo_s {
	int si_signo;
	int si_errno;
	int si_code;

	union {
		int _pad[SI_PAD_SIZE];

		/* kill() */
		struct {
			int _pid;		/* sender's pid */
			int _uid;		/* sender's uid */
		} _kill;

		/* POSIX.1b timers */
		struct {
			unsigned int _timer1;
			unsigned int _timer2;
		} _timer;

		/* POSIX.1b signals */
		struct {
			int _pid;		/* sender's pid */
			int _uid;		/* sender's uid */
			sigval_t _sigval;
		} _rt;

		/* SIGCHLD */
		struct {
			int _pid;		/* which child */
			int _uid;		/* sender's uid */
			int _status;		/* exit code */
			long _utime;		/* clock_t */
			long _stime;		/* clock_t */
		} _sigchld;

		/* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
		struct {
			void *_addr; /* faulting insn/memory ref. */
		} _sigfault;

		/* SIGPOLL */
		struct {
			int _band;	/* POLL_IN, POLL_OUT, POLL_MSG */
			int _fd;
		} _sigpoll;
	} _sifields;
} siginfo_t;

struct signal_s {
    int signal_nr;
    int signal_prio;
    siginfo_t siginfo;
    struct list_head list;    
};


/*
 * How these fields are to be accessed.
 */
#define si_pid		_sifields._kill._pid
#define si_uid		_sifields._kill._uid
#define si_status	_sifields._sigchld._status
#define si_utime	_sifields._sigchld._utime
#define si_stime	_sifields._sigchld._stime
#define si_value	_sifields._rt._sigval
#define si_int		_sifields._rt._sigval.sival_int
#define si_ptr		_sifields._rt._sigval.sival_ptr
#define si_addr		_sifields._sigfault._addr
#define si_band		_sifields._sigpoll._band
#define si_fd		_sifields._sigpoll._fd

#ifdef __KERNEL__
#define __SI_MASK	0xffff0000
#define __SI_KILL	(0 << 16)
#define __SI_TIMER	(1 << 16)
#define __SI_POLL	(2 << 16)
#define __SI_FAULT	(3 << 16)
#define __SI_CHLD	(4 << 16)
#define __SI_RT		(5 << 16)
#define __SI_CODE(T,N)	((T) << 16 | ((N) & 0xffff))
#else
#define __SI_KILL	0
#define __SI_TIMER	0
#define __SI_POLL	0
#define __SI_FAULT	0
#define __SI_CHLD	0
#define __SI_RT		0
#define __SI_CODE(T,N)	(N)
#endif

/*
 * si_code values
 * Digital reserves positive values for kernel-generated signals.
 */
#define SI_USER		0		/* sent by kill, sigsend, raise */
#define SI_KERNEL	0x80		/* sent by the kernel from somewhere */
#define SI_QUEUE	-1		/* sent by sigqueue */
#define SI_TIMER __SI_CODE(__SI_TIMER,-2) /* sent by timer expiration */
#define SI_MESGQ	-3		/* sent by real time mesq state change */
#define SI_ASYNCIO	-4		/* sent by AIO completion */
#define SI_SIGIO	-5		/* sent by queued SIGIO */
#define SI_TKILL	-6		/* sent by tkill system call */

#define SI_FROMUSER(siptr)	((siptr)->si_code <= 0)
#define SI_FROMKERNEL(siptr)	((siptr)->si_code > 0)

/*
 * SIGILL si_codes
 */
#define ILL_ILLOPC	(__SI_FAULT|1)	/* illegal opcode */
#define ILL_ILLOPN	(__SI_FAULT|2)	/* illegal operand */
#define ILL_ILLADR	(__SI_FAULT|3)	/* illegal addressing mode */
#define ILL_ILLTRP	(__SI_FAULT|4)	/* illegal trap */
#define ILL_PRVOPC	(__SI_FAULT|5)	/* privileged opcode */
#define ILL_PRVREG	(__SI_FAULT|6)	/* privileged register */
#define ILL_COPROC	(__SI_FAULT|7)	/* coprocessor error */
#define ILL_BADSTK	(__SI_FAULT|8)	/* internal stack error */
#define NSIGILL		8

/*
 * SIGFPE si_codes
 */
#define FPE_INTDIV	(__SI_FAULT|1)	/* integer divide by zero */
#define FPE_INTOVF	(__SI_FAULT|2)	/* integer overflow */
#define FPE_FLTDIV	(__SI_FAULT|3)	/* floating point divide by zero */
#define FPE_FLTOVF	(__SI_FAULT|4)	/* floating point overflow */
#define FPE_FLTUND	(__SI_FAULT|5)	/* floating point underflow */
#define FPE_FLTRES	(__SI_FAULT|6)	/* floating point inexact result */
#define FPE_FLTINV	(__SI_FAULT|7)	/* floating point invalid operation */
#define FPE_FLTSUB	(__SI_FAULT|8)	/* subscript out of range */
#define NSIGFPE		8

/*
 * SIGSEGV si_codes
 */
#define SEGV_MAPERR	(__SI_FAULT|1)	/* address not mapped to object */
#define SEGV_ACCERR	(__SI_FAULT|2)	/* invalid permissions for mapped object */
#define NSIGSEGV	2

/*
 * SIGBUS si_codes
 */
#define BUS_ADRALN	(__SI_FAULT|1)	/* invalid address alignment */
#define BUS_ADRERR	(__SI_FAULT|2)	/* non-existant physical address */
#define BUS_OBJERR	(__SI_FAULT|3)	/* object specific hardware error */
#define NSIGBUS		3

/*
 * SIGTRAP si_codes
 */
#define TRAP_BRKPT	(__SI_FAULT|1)	/* process breakpoint */
#define TRAP_TRACE	(__SI_FAULT|2)	/* process trace trap */
#define NSIGTRAP	2

/*
 * SIGCHLD si_codes
 */
#define CLD_EXITED	(__SI_CHLD|1)	/* child has exited */
#define CLD_KILLED	(__SI_CHLD|2)	/* child was killed */
#define CLD_DUMPED	(__SI_CHLD|3)	/* child terminated abnormally */
#define CLD_TRAPPED	(__SI_CHLD|4)	/* traced child has trapped */
#define CLD_STOPPED	(__SI_CHLD|5)	/* child has stopped */
#define CLD_CONTINUED	(__SI_CHLD|6)	/* stopped child has continued */
#define NSIGCHLD	6

/*
 * SIGPOLL si_codes
 */
#define POLL_IN		(__SI_POLL|1)	/* data input available */
#define POLL_OUT	(__SI_POLL|2)	/* output buffers available */
#define POLL_MSG	(__SI_POLL|3)	/* input message available */
#define POLL_ERR	(__SI_POLL|4)	/* i/o error */
#define POLL_PRI	(__SI_POLL|5)	/* high priority input available */
#define POLL_HUP	(__SI_POLL|6)	/* device disconnected */
#define NSIGPOLL	6

int signal_send(int pid, int sig, siginfo_t *siginfo);
int signal_queue(struct signals_s *signals, int sig, struct signal_s *signal, struct siginfo_s *siginfo);
int signals_fork(struct task_s *parent, struct task_s *task);
struct signal_s *signal_dequeue(struct task_s *task);

#include <agnix/signals/signals.h>

#endif
