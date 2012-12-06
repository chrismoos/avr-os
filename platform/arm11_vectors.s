.global vectors_start
.global vectors_end

.code 32

vectors_start:

ldr pc, reset_addr
ldr pc, interrupt_hdlr_addr
ldr pc, swi_hdlr_addr /* SWI */
ldr pc, interrupt_hdlr_addr
ldr pc, interrupt_hdlr_addr
ldr pc, interrupt_hdlr_addr
ldr pc, interrupt_hdlr_addr

interrupt_hdlr_addr: .word interrupt_hdlr
swi_hdlr_addr: .word swi_handler
reset_addr: .word _Reset

vectors_end:
