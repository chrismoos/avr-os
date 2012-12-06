ENTRY(_Reset)
SECTIONS
{
 . = 0x10000;
 .startup . : { build/start.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x4000; /* 4kB of stack memory */
 stack_top = .;
 . = . + 0x4000; /* 4kB of interrupt stack memory */
 svc_stack_top = .;
 . = . + 0x4000; /* 4kB of interrupt stack memory */
 isr_stack_top = .;
 . = . + 0x4000; /* little buffer of 4kB */
 task_stack_top = .;
 . = . + 0x800000;
 heap_low = .;
 . = . + 0x10000;
 heap_top = .;
}
