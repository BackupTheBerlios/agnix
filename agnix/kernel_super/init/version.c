/*
 *  linux/init/version.c
 *
 *  Copyright (C) 1992  Theodore Ts'o
 * 
 *  Changes:
 *  Date:	2004/01
 *  Author: 	Lukasz Dembinski
 *  Info:	
 *  Contact:	mailto: <dembol@linux.net>
 *
 */

#include <agnix/uts.h>
#include <agnix/utsname.h>
#include <agnix/version.h>
#include <agnix/compile.h>

#define version(a) Version_ ## a
#define version_string(a) version(a)

const char *agnix_banner = 
	"Agnix release: " UTS_RELEASE " (" AGNIX_COMPILE_BY "@"
	AGNIX_COMPILE_HOST ") (" AGNIX_COMPILER ")\n";

const char *agnix_version = 
	"Agnix version: " UTS_VERSION "\n";
	
const char *agnix_license = 
	"Agnix license: " AGNIX_LICENSE "\n";
