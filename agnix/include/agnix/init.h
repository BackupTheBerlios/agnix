#ifndef _AGNIX_INIT_H_
#define _AGNIX_INIT_H_

#include <agnix/config.h>

#ifndef MODULE

#define __initcall(fn) static initcall_t __initcall_##fn __init_call = fn
#define __exitcall(fn) static exitcall_t __exitcall_##fn __exit_call = fn
#define __init		__attribute__ ((__section__ (".text.init")))
#define __exit		__attribute__ ((unused, __section__(".text.exit")))

//#define __INIT		.section	".text.init","ax"
//#define __FINIT		.previous
//#define __INITDATA	.section	".data.init","aw"

#else

#define __init
#define __exit
#define __INIT
#define __FINIT
#define __INITDATA

#endif

#endif
