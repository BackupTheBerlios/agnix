/*
 * kernel_libs/other/vsprintf.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 * 
 * Date:	2004/01
 * Author: 	Lukasz Dembinski
 * Info:	vsprintf.c core file
 * Contact:	mailto: <dembol@linux.net>
 *
 */

#include <stdarg.h>
#include <agnix/types.h>
#include <agnix/memory.h>

#define SX_SMALL	1
#define SX_BIG		2
#define SS_NORMAL	3
#define SS_UPPER	4

#define TAB_LEN		3
#define MAX_NUM_STR	32

#define DIGIT_D(c) 	 (c >= '0' && c <= '9')
#define DIGIT_H(c)	((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || \
			 (c >= 'a' && c <= 'f'))

unsigned long convert_str_to_num(const char *fmtt, int len, unsigned char base)
{
    unsigned long liczba = 0;
    unsigned char val;
    int i = len - 1;
    unsigned long cur_base = 1;
    
    do {
    	if (DIGIT_D(fmtt[i])) {
	    val = fmtt[i] - '0';
	    liczba += (unsigned long)(val * cur_base);
	    cur_base *= base;
	} else
	    break;
	    
	i--;
    } while ((i >= 0) && (fmtt[i] != 0));
    
    return liczba;    
}

char *convert_num_to_str(char *buf, unsigned long number, unsigned char base, 
			 int number_len, int size_ch, char which_ch)
{
    char num_str[MAX_NUM_STR];
    unsigned long rest;
    unsigned long liczba;
    char digit;
    int dig_nr = 0;
    int i = 0;

    memset(num_str, 0, MAX_NUM_STR);

    liczba = number;
    do {
    	rest   = liczba % base;
	liczba = liczba / base;
	
	if (base > 10) {
	    if (rest >= 10) {
		if (size_ch == SX_SMALL)
		    digit = (rest - 10) + 'a';
		else
		    digit = (rest - 10) + 'A';
	    } else
	        digit = rest + '0';
	}
	else {
	    digit = rest + '0';
	}
    
	num_str[dig_nr++] = digit;
    
    } while(liczba > 0);

    if (number_len > 0) {
	for (i = 0; i < number_len; i++)
	    buf[i] = which_ch;
    }

    dig_nr--;
    
    if (number_len > 0) {
        number_len--;

	if (dig_nr > number_len)
	    dig_nr = number_len;

	i = number_len - dig_nr;
    }

    buf += i;
    for (; dig_nr >=0; dig_nr--) {
	*buf = num_str[dig_nr];
	buf++;
    }
    
    return buf;
}

char *convert_str_to_str(char *buf, char *ptr, int size_ch)
{
    if (ptr == 0) {
	memcpy(buf, "<NULL>", 6);
	buf += 6;
	return buf;
    }

    do {
	if (*ptr != 0) {
	    *buf = *ptr;
	    buf++;
	    ptr++;
	}
    } while(*ptr);
    
    return buf;
}

int vsnprintf(char *buf, size_t size, const char *fmt_ptr, va_list args)
{
    char *tmp;
    const char *fmtt_ptr, *fmtt_tmp;
    unsigned char base;
    unsigned char string;
    int size_ch = 0;
    unsigned long number;
    char asciinumber;
    char *ptr;
    int ile;
    int  number_len;
    char which_ch;
    
    tmp = buf;
    fmtt_ptr = fmt_ptr;

    do {
	if (*fmtt_ptr != '%') {
	
	    if (*fmtt_ptr == '\t') {	
		ile = TAB_LEN - ((int)(tmp - buf) & TAB_LEN);
		
		for(; ile >= 0; ile--) {
		    *tmp = ' ';
		    tmp++;
		}
				
		fmtt_ptr++;
		continue;
	    }
	
	    *tmp = *fmtt_ptr;
	    tmp++;
	    
	    if (*fmtt_ptr == 0)
		break;

	    fmtt_ptr++;
	    continue;
	}

	fmtt_ptr++;
	
	number_len = 0;
	which_ch   = 0;
	
	if (DIGIT_D(*fmtt_ptr)) {

	    switch(*fmtt_ptr) {
		case '0':
		    which_ch = '0';
		    fmtt_ptr++; break;
		default:
		    which_ch = ' ';
	    }
		
	    ile = 0;    
	    fmtt_tmp = fmtt_ptr;
	    do {
		if ((*fmtt_tmp == 0) || (!DIGIT_D(*fmtt_tmp)))
		    break;
	    
	        ile++;
		fmtt_tmp++;
	    } while(DIGIT_D(*fmtt_tmp) && (*fmtt_tmp != 0));	    
	    	    
	    number_len = convert_str_to_num(fmtt_ptr, ile, 10);
	    fmtt_ptr = fmtt_tmp;
	}

	if (*fmtt_ptr == 0) {
	    *tmp = *fmtt_ptr;
	    break;
	}

	base = 0; string = 0;
	switch (*fmtt_ptr) {
	    case 'd': base = 10;
		      break;

	    case 'x': base = 16;
		      size_ch = SX_SMALL;
		      break;

	    case 'X': base = 16;
		      size_ch = SX_BIG;
		      break;

	    case 's': string = 1;
		      size_ch = SS_NORMAL;
		      break;

	    case 'S': string = 1;
		      size_ch = SS_UPPER;
		      break;

	    case 'c':
		      asciinumber = (unsigned char)va_arg(args, int);
		      *(tmp++) = asciinumber;	
		      break;

	    default:
		      break;
	}	
	
	if (base > 0) {
	    number = va_arg(args, long);
	    tmp = convert_num_to_str(tmp, number, base, number_len, size_ch, which_ch);
	}
	
	if (string) {
	    ptr = va_arg(args, char *);
	    tmp = convert_str_to_str(tmp, ptr, size_ch);
	}

	fmtt_ptr++;
	if (*fmtt_ptr == 0)
	    *tmp = *fmtt_ptr;
    
    } while (*fmtt_ptr != 0);

    return tmp-buf;
}

int snprintf(char *buf, size_t size, const char *fmt_ptr, ...)
{
	va_list args;
	int i;

	va_start(args, fmt_ptr);
	i=vsnprintf(buf, size, fmt_ptr, args);
	va_end(args);
	
	return i;
}

int sprintf(char *buf, const char *fmt_ptr, ...)
{
	va_list args;
	int i;

	va_start(args, fmt_ptr);
        i = vsnprintf(buf, 0xFFFFFFFFUL, fmt_ptr, args);
	va_end(args);
	
	return i;
}

int vsprintf(char *buf, const char *fmt_ptr, va_list args)
{
	return vsnprintf(buf, 0xFFFFFFFFUL, fmt_ptr, args);
}

