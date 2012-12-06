.global _Reset
_Reset:
 LDR sp, =stack_top

 MRS r1, cpsr
 BIC r1, r1, #0x80
 mov r2, r1
 MSR cpsr_c, r1 

 and r1, #0x12
 MSR cpsr_c, r1
 LDR sp, =isr_stack_top

 /* set supervisor stack */
 and r1, #0x13
 MSR cpsr_c, r1
 LDR sp, =svc_stack_top

 MSR cpsr_c, r2

 BL main
 B .
