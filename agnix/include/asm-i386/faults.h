#ifndef _AGNIX_ASM_FAULTS_H_
#define _AGNIX_ASM_FAULTS_H_

#include <asm/types.h>
#include <agnix/linkage.h>

asmlinkage void TRAP_divide_fault(u32 error);
asmlinkage void TRAP_debug(u32 error);
asmlinkage void TRAP_nmi(u32 error);
asmlinkage void TRAP_int_3(u32 error);
asmlinkage void TRAP_overflow(u32 error);
asmlinkage void TRAP_bounds(u32 error);
asmlinkage void TRAP_invalid_op(u32 error);
asmlinkage void TRAP_dev_not_avail(u32 error);
asmlinkage void TRAP_double_fault(u32 error);
asmlinkage void TRAP_coprocessor_seg_overrun(u32 error);
asmlinkage void TRAP_invalid_TSS(u32 error);
asmlinkage void TRAP_seg_not_present(u32 error);
asmlinkage void TRAP_stack_seg(u32 error);
asmlinkage void TRAP_general_protection(u32 error);
asmlinkage void TRAP_page_fault(u32 error);
asmlinkage void TRAP_reserved(u32 error);
asmlinkage void TRAP_coprocessor_error(u32 error);
asmlinkage void TRAP_align_check(u32 error);

int faults_init(void);

#endif
