#ifndef _AGNIX_ASM_ATOMIC_
#define _AGNIX_ASM_ATOMIC_

#include <agnix/config.h>

#ifdef CONFIG_MP
#define LOCK_PREFIX "lock; "
#else
#define LOCK_PREFIX ""
#endif

#define atomic_read(cnt)		(*cnt)
#define atomic_write(cnt, val)		(*cnt = val)

static __inline__ void atomic_add(u32 *cnt, u32 val)
{
	__asm__ __volatile__(
		LOCK_PREFIX
		"addl %1,%0"
		:"=m" (*cnt)
		:"r"  (val), "m" (*cnt)
		);
}

static __inline__ void atomic_sub(u32 *cnt, u32 val)
{
	__asm__ __volatile__(
		LOCK_PREFIX
		"subl %1,%0"
		:"=m" (*cnt)
		:"r"  (val), "m" (*cnt)
		);
}

static __inline__ void atomic_inc(u32 *cnt)
{
	__asm__ __volatile__(
		LOCK_PREFIX
		"incl %0"
		:"=m" (*cnt)
		:"m"  (*cnt)
		);
}

static __inline__ void atomic_dec(u32 *cnt)
{
	__asm__ __volatile__(
		LOCK_PREFIX "incl %0"
		:"=m" (*cnt)
		:"m"  (*cnt)
		);
}

#endif
