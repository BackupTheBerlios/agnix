/*
 * kernel_drivers/adi_char/terminal.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	terminal.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/agnix.h>
#include <agnix/terminal.h>
#include <agnix/resources.h>
#include <agnix/list.h>
#include <agnix/spinlock.h>
#include <agnix/console.h>

#define MOD_NAME		"TERM: "

LIST_HEAD(terminal_list);
LIST_HEAD(terminal_sys_list);
spinlock_t terminal_list_lock;

extern struct terminal_s term_sys[MAX_TERMINALS];
extern int term_cur;
int term_nr = 0;

int terminal_switch(int term_switch)
{
    struct terminal_s *term = &term_sys[term_cur];
    struct terminal_s *term_new = &term_sys[term_switch];
    
    if (term_new->term_driver && term_new->term_driver->ops->refresh) {
    	term_cur 		= term_switch;
	term_new->term_driver->ops->refresh(term_new, 0);
	term->term_active 	= 0;
	term_new->term_active 	= 1;
    }
    
    return 0;    
}

int put_free_terminal(int terminal_desc)
{
    return 0;
}

int get_free_terminal(void)
{
    return term_nr++;
}


/*int terminal_nr = 0;

int get_free_terminal(void)
{
    return terminal_nr++;
}

void put_free_terminal(void)
{
}
*/

int set_terminal_var(int term_nr, struct terminal_var_s *term_var)
{
    return 0;    
}

int register_terminal(struct terminal_s *term)
{
    term->term_nr = get_free_terminal();

    if (term->ops && term->ops->init)
	term->ops->init(term);

    list_add(&term->term_list, &terminal_list);

    return term->term_nr;
}

int register_sys_terminal(struct terminal_s *term)
{
    spin_lock(&terminal_list_lock);
    list_add(&term->term_list, &terminal_sys_list);
    spin_unlock(&terminal_list_lock);

    return 0;
}

int register_terminal_driver(struct terminal_s *term, struct terminal_driver_s *term_driver)
{
    term->term_driver = term_driver;
    
    if (term_driver->ops && term_driver->ops->init)
	term_driver->ops->init();

    return 0;
}

int terminals_init(void)
{
    spin_lock_init(&terminal_list_lock);
    
    return 0;
}
