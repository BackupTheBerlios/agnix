#ifndef _AGNIX_TYPES_H_
#define _AGNIX_TYPES_H_

#include <asm/types.h>

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int ssize_t;
#endif

typedef long clock_t;

typedef	unsigned short	dev_t;		/* device id */
typedef	unsigned long	gid_t;		/* group id */
typedef	unsigned long	ino_t;		/* inode number */
typedef	unsigned short	mode_t;		/* permissions */
typedef	unsigned short	nlink_t;	/* link count */
typedef	unsigned int	off_t;		/* file offset */
typedef	unsigned long	uid_t;		/* user id */

/* Symbols allowed but not required by POSIX */

typedef	char *		caddr_t;	/* address of a (signed) char */

#ifndef _TIME_T
#define	_TIME_T
typedef	unsigned int	time_t;
#endif

#define	major(i)	(((i) >> 8) & 0xFF)
#define	minor(i)	((i) & 0xFF)
#define	makedev(i,j)	((((i) & 0xFF) << 8) | ((j) & 0xFF))

#ifndef	NULL
#define	NULL		((void *) 0) 	/* the null pointer */
#endif

#endif

