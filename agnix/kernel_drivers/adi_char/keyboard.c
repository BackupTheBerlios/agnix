/*
 * kernel_drivers/adi_char/keyboard.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	keyboard.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

/*
 * TODO:
 * - register_kbd_action()
 * - unregister_kbd_action()
 */

#include <agnix/agnix.h>
#include <agnix/init.h>
#include <agnix/ioport.h>
#include <agnix/console.h>
#include <agnix/terminal.h>
#include <agnix/memory.h>
#include <agnix/irq.h>
#include <agnix/machine.h>

#define MOD_NAME		"CHAR_KBD: \t"

#define	KBD_DTA_PORT		0x60
#define	KBD_CMD_PORT		0x64
#define	KBD_STS_PORT		0x64

#define KBD_DCMD_READID		0xF2
#define KBD_DCMD_SETDELAY	0xF3
#define KBD_DCMD_ENABLE		0xF4
#define KBD_DCMD_DISABLE	0xF5
#define KBD_DCMD_RESET		0xFF

#define KBD_CCMD_WRITEMODE	0x60
#define KBD_CCMD_SELFTEST	0xAA
#define KBD_CCMD_IFACETEST	0xAB
#define KBD_CCMD_SETLEDS	0xED

#define KBD_WAIT_LOOPS		10000000
#define KBD_STS_OBF		0x01
#define KBD_STS_IBF		0x02
#define KBD_STS_PERR		0x80
#define KBD_STS_GTMO		0x40

#define KBD_IRQ			1

#define SPECIAL			0
#define KBD_CODE_ESC		0x01
#define KBD_CODE_BACKSPACE	0x0E
#define KBD_CODE_TAB		0x0F
#define KBD_CODE_CTRL		0x1D
#define KBD_CODE_SHIFTL		0x2A
#define KBD_CODE_SHIFTR		0x36
#define KBD_CODE_PRINT		0x37
#define KBD_CODE_ALT		0x38
#define KBD_CODE_CAPS		0x3A
#define KBD_CODE_SCROLL		0x45
#define KBD_CODE_NUM		0x46

#define KBD_CODE_F1		0x3B
#define KBD_CODE_F2		0x3C
#define KBD_CODE_F3		0x3D
#define KBD_CODE_F4		0x3E
#define KBD_CODE_F5		0x3F
#define KBD_CODE_F6		0x40
#define KBD_CODE_F7		0x41
#define KBD_CODE_F8		0x42
#define KBD_CODE_F9		0x43
#define KBD_CODE_F10		0x44
//#define KBD_CODE_F11		0x3B
//#define KBD_CODE_F12		0x3B

#define ENTER			10
#define F1			'A'
#define F2			'B'
#define F3			'C'
#define F4			'D'
#define F5			'E'
#define F6			'F'
#define F7			'G'
#define F8			'H'
#define F9			'I'
#define F10			'J'
#define F11			'K'
#define F12			'L'
#define PAUSE			'P'

#define KBD_LED_SCROLL		0x01
#define KBD_LED_NUM		0x02
#define KBD_LED_CAPS		0x04

#define KBD_FLG_SCROLL		KBD_LED_SCROLL
#define KBD_FLG_NUM		KBD_LED_NUM
#define KBD_FLG_CAPS		KBD_LED_CAPS
#define KBD_FLG_SHIFT		0x08
#define KBD_FLG_CNTRL		0x10
#define KBD_FLG_ALT		0x20

#define MAX_KBD_ACTIONS		16

extern int term_cur;
extern struct terminal_s term_sys[MAX_TERMINALS];

u8 kbd_flags = 0;

int kbd_action_count = 0;

struct kbd_action_s {
    u8 active_flags;
    u8 passive_flags;
    u8 scancode;
    int (*kbd_action_routine)(int scancode);
};

struct kbd_action_s kbd_action[MAX_KBD_ACTIONS];

char scancode_to_ascii[0x100] = {
    SPECIAL, SPECIAL, 
    '1','2','3','4','5','6','7','8','9','0','-','=',
    SPECIAL, SPECIAL, 
    'q','w','e','r','t','y','u','i','o','p','[',']',ENTER,SPECIAL,
    'a','s','d','f','g','h','j','k','l',';','\'',SPECIAL,'\\',
    '<','z','x','c','v','b','n','m',',','.','/',SPECIAL,SPECIAL,SPECIAL,
    ' ',SPECIAL,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PAUSE,SPECIAL,SPECIAL,    
};

char scancode_to_ascii_shift[0x100] = {
    SPECIAL, SPECIAL, 
    '!','@','#','$','%','^','&','*','(',')','_','+',
    SPECIAL, SPECIAL, 
    'Q','W','E','R','T','Y','U','I','O','P','{','}',ENTER,SPECIAL,
    'A','S','D','F','G','H','J','K','L',':','"',SPECIAL,'|',
    '>','Z','X','C','V','B','N','M','<','>','?',SPECIAL,SPECIAL,SPECIAL,
    ' ',SPECIAL,'~','~','~','~','~','~','~','~','~','~','~','~',PAUSE,SPECIAL,SPECIAL,    
};



u8 kbd_status_read(void)
{
    u8 status;
    
    status = inb(KBD_STS_PORT);
    
    return status;
}

int kbd_wait_for_bf(u8 which_bf, int neg)
{
    u8 status;
    int wait_loops = KBD_WAIT_LOOPS;
    
    for (; wait_loops >= 0; wait_loops--) {
        status = kbd_status_read();
	
	if (neg) {
	    if (!(status & which_bf))
	        return 1;
	}
	else {
	    if (status & which_bf)
		return 1;
	}
    }

    return 0;
}

int kbd_wait_for_notibf(void)
{
    return kbd_wait_for_bf(KBD_STS_IBF, 1);
}

int kbd_wait_for_obf(void)
{
    return kbd_wait_for_bf(KBD_STS_OBF, 0);
}

void kbd_data_write(u8 cmd)
{
    kbd_wait_for_notibf();
    outb(cmd, KBD_DTA_PORT);
}

char kbd_data_read(void)
{
    u8 status;
    char data = -1;

    status = kbd_status_read();
    
    if ((status & KBD_STS_OBF) && !(status & (KBD_STS_PERR | KBD_STS_GTMO))) {

        data = inb(KBD_DTA_PORT);
    }
    
    return data;
}

char kbd_data_read_wait(void)
{
    char data = -1;
    
    if (kbd_wait_for_obf())
	data = kbd_data_read();

    return data;
}

void kbd_cmd_write(u8 cmd)
{
    kbd_wait_for_notibf();
    outb(cmd, KBD_CMD_PORT);
}

void kbd_set_leds(u8 leds_mask)
{
    kbd_data_write(KBD_CCMD_SETLEDS);
    kbd_data_write(leds_mask);
}

int kbd_change_led_flag(u8 led_flag)
{
    if (kbd_flags & led_flag)
	kbd_flags &= ~led_flag;
    else
	kbd_flags |= led_flag;
	
    return 1;
}

void kbd_irq_handler(u32 data)
{
    u8 status;
    u8 scancode;
    char asciicode;
    int leds_changed = 0;
    int i, k;

    status = kbd_status_read();

    for (k = 0; k < 10000; k++) {    
    if ((status & KBD_STS_OBF) && !(status & (KBD_STS_PERR | KBD_STS_GTMO))) {

        scancode = inb(KBD_DTA_PORT);

	switch(scancode) {
	    case KBD_CODE_CTRL:
		    kbd_flags |= KBD_FLG_CNTRL;
		    break;
	    case KBD_CODE_CTRL+128:
		    kbd_flags &= ~KBD_FLG_CNTRL;
		    break;
	    case KBD_CODE_ALT:
		    kbd_flags |= KBD_FLG_ALT;
		    break;
	    case KBD_CODE_ALT+128:
		    kbd_flags &= ~KBD_FLG_ALT;
		    break;
	    case KBD_CODE_CAPS:
		    leds_changed = kbd_change_led_flag(KBD_FLG_CAPS);
		    break;
	    case KBD_CODE_SCROLL:
		    leds_changed = kbd_change_led_flag(KBD_FLG_SCROLL);
		    break;
	    case KBD_CODE_NUM:
		    leds_changed = kbd_change_led_flag(KBD_FLG_NUM);
		    break;
	    case KBD_CODE_SHIFTR:
	    case KBD_CODE_SHIFTL:
		    kbd_flags |= KBD_FLG_SHIFT;
		    break;
	    case KBD_CODE_SHIFTR+128:
            case KBD_CODE_SHIFTL+128:
		    kbd_flags &= ~KBD_FLG_SHIFT;
		    break;
	    default:

		if ((scancode >= KBD_CODE_F1) && (scancode <= KBD_CODE_F10) &&
		     (kbd_flags & KBD_FLG_ALT)) {
		     terminal_switch(scancode - KBD_CODE_F1);
		     break; 
		}

		if (scancode < 128) {

		    if ((kbd_flags & KBD_FLG_SHIFT) || (kbd_flags & KBD_FLG_CAPS))
			    asciicode = scancode_to_ascii_shift[scancode];
		    else
			    asciicode = scancode_to_ascii[scancode];

		    if (asciicode != 0) {
			if (term_sys[term_cur].ops && term_sys[term_cur].ops->write_char)
			    term_sys[term_cur].ops->write_char(&term_sys[term_cur], asciicode);
//			    printk("%c", asciicode);
		    }
		}
	    }

        for (i = 0; i < kbd_action_count; i++) {
	    if (((!kbd_action[i].scancode) || (kbd_action[i].scancode == scancode)) &&
    	        ((kbd_action[i].active_flags & kbd_flags) == kbd_action[i].active_flags)) {
    		kbd_action[i].kbd_action_routine(scancode);
    	    }
	}

	if (leds_changed)
    	    kbd_set_leds(kbd_flags & 0x07);

	break;
    }
    }

    if (k == 10000) 
        printk("Keyboard interrupt error!\n");
}

int kbd_screen_scroll = 0;

int kbd_screen_up(int scancode)
{
    kbd_screen_scroll++;

    term_sys[term_cur].term_driver->ops->refresh(&term_sys[term_cur], kbd_screen_scroll);    

    return 0;
}

int kbd_screen_down(int scancode)
{
    if (kbd_screen_scroll > 0) {
        kbd_screen_scroll--;
	term_sys[term_cur].term_driver->ops->refresh(&term_sys[term_cur], kbd_screen_scroll);    
    }

    return 0;
}

int cad_power_down(int scancode)
{
    do_machine_reboot();
//    do_machine_powerdown();

    return 0;
}

int kbd_set_actions(void)
{
    memset(kbd_action, 0, sizeof(struct kbd_action_s) * MAX_KBD_ACTIONS);
    
    kbd_action[0].active_flags = 0;
    kbd_action[0].passive_flags = 0;
    kbd_action[0].scancode = 73;	/* PAGE_UP */
    kbd_action[0].kbd_action_routine = kbd_screen_up;

    kbd_action[1].active_flags = 0;
    kbd_action[1].passive_flags = 0;
    kbd_action[1].scancode = 81;	/* PAGE_DOWN */
    kbd_action[1].kbd_action_routine = kbd_screen_down;

    kbd_action[2].active_flags = KBD_FLG_ALT | KBD_FLG_CNTRL;
    kbd_action[2].passive_flags = 0;
    kbd_action[2].scancode = 0x53;	/* DEL */
    kbd_action[2].kbd_action_routine = cad_power_down;

    kbd_action_count = 3;

    return 0;
}

struct irq_routine_s kbd_irq_routine = {
    kbd_irq_handler, 0, IRQ_FLAG_RANDOM, 
};

int adi_chrdrv_kbd_init(void)
{
    char data;

    printk(MOD_NAME "PC keyboard ... ");

    kbd_cmd_write(KBD_CCMD_SELFTEST);
    if ((data=kbd_data_read_wait()) != 0x55) {
	printk("Your keyboard is failed!\n");
	return -1;
    }

    kbd_cmd_write(KBD_CCMD_IFACETEST);
    if (kbd_data_read_wait() != 0x00) {
	printk("Your keyboard is failed!\n");
	return -1;
    }

    printk("ok\n");

    kbd_cmd_write(KBD_CCMD_WRITEMODE);
    kbd_data_write(0x40 | 0x20 | 0x04 | 0x01);

//    kbd_data_write(KBD_DCMD_RESET);
    kbd_data_write(KBD_DCMD_ENABLE);

    kbd_set_leds(0x00);

    kbd_set_actions();

    install_irq(KBD_IRQ, &kbd_irq_routine);

    return 0;
}
