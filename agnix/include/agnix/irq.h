#ifndef _AGNIX_IRQ_H_
#define _AGNIX_IRQ_H_

#include <asm/irq.h>
#include <agnix/list.h>
#include <asm/types.h>

#define IRQ_FLAG_RANDOM		0x01

struct irq_routine_s {
    void (*proc)(u32 data);
    u32 data;
    u32 flags;
    struct list_head list;
};

struct irq_s {
    struct list_head head;
    u32 status;
    u32 flags;
};

int install_irq(u8 irq, struct irq_routine_s *irq_proc); 

#endif
