#ifndef _AGNIX_IOPORT_H_
#define _AGNIX_IOPORT_H_

#include <asm/types.h>

#define INP(s, b) \
extern b in##s(unsigned short port)

#define OUTP(s, b) \
extern void out##s(b val, unsigned short port)

INP(b, u8 );
INP(w, u16);
INP(l, u32);

OUTP(b, u8 );
OUTP(w, u16);
OUTP(l, u32);

#endif
