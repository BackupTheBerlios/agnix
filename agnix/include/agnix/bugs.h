//#ifndef _AGNIX_ASM_BUGS_H_
//#define _AGNIX_ASM_BUGS_H_

#define BUG_FREE_PAGES		"free_pages"
#define BUG_RESERVE_PAGES	"reserve_pages"

#define BUG(x,y ) \
    do { printk(x y"() [%s (line %d)]\n", __FILE__, __LINE__); \
       } while(0)

//#endif
