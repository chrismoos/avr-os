/* ========================================================================
 * Copyright 2012 Chris Moos
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
 */

#include "../os.h"
#include "../os_internal.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define cli(); asm volatile("CPSID i");
#define sei(); asm volatile("CPSIE i");

volatile unsigned int *UART0DR = (unsigned int *)0x101f1000;

volatile uint32_t *SYST_CTR = (uint32_t*)0xE000E010;
volatile uint32_t *SYST_RVR = (uint32_t*)0xE000E014;
volatile uint32_t *SYST_CALIB = (uint32_t*)0xE000E01C;

#define REG_UL(x) (*((volatile unsigned long*)(x)))
#define SYSTICK_CTRL REG_UL(0xe000e010)

typedef void(*interrupt_handler)();

#define VECTOR_OFFSET(N) (uint32_t*)((N - 1) * 4)

#define TIMER_0_VECTOR_OFFSET VECTOR_OFFSET(40)

/* http://infocenter.arm.com/help/topic/com.arm.doc.dui0224i/I1042232.html */
#define PIC ((volatile unsigned int*)0x10120000)
#define PIC_TIMER01 0x10
/* http://infocenter.arm.com/help/topic/com.arm.doc.ddi0181e/I1006461.html */
#define VIC_INTENABLE 0x4 /* 0x10 bytes */

#define TIMER0_BASE (uint32_t*)0x101e2000

#define PIC_BASE (uint32_t*)0x10140000
#define PIC_INT_ENABLE (uint32_t*)0x10140010

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/index.html */
#define TIMER0 ((volatile unsigned int*)0x101E2000)
#define TIMER_VALUE 0x1 /* 0x04 bytes */
#define TIMER_CONTROL 0x2 /* 0x08 bytes */
#define TIMER_INTCLR 0x3 /* 0x0C bytes */
#define TIMER_MIS 0x5 /* 0x14 bytes */
/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/Babgabfg.html */
#define TIMER_EN 0x80
#define TIMER_PERIODIC 0x40
#define TIMER_INTEN 0x20
#define TIMER_32BIT 0x02
#define TIMER_ONESHOT 0x01

extern uint32_t task_stack_top;
extern uint32_t isr_stack_top;

uint32_t *task_stack_top_ptr = &task_stack_top;

volatile uint32_t *VECTOR_TABLE = (uint32_t*)0;



int print_uart0(const char *s) {
 int x = 0; 
 while(*s != '\0') { /* Loop until end of string */
 *UART0DR = (unsigned int)(*s); /* Transmit char */
 x++;
 s++; /* Next char */
 }
 return x;
}

int puts(const char *s) {
    return print_uart0(s);
}

void setup_vectors() {
    extern uint32_t vectors_start;
    extern uint32_t vectors_end;

    uint32_t *vectors_src = &vectors_start;
    uint32_t *vectors_dst = (uint32_t*)0;

    while(vectors_src < &vectors_end) {
        *vectors_dst++ = *vectors_src;
        vectors_src++;
    }
}

void _os_platform_init() {
    setup_vectors();

    // enable interrupts for timer 0 and 1
    *(PIC_INT_ENABLE) |= (1 << 4) | (1 << 5);
    *(PIC + VIC_INTENABLE) = PIC_TIMER01;

    *TIMER0 = 10000;
    *(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
}

void _os_platform_loop() {
    while(1) {
        asm volatile("wfi");
	}
}

void _os_platform_sleep() {
    asm volatile("wfi");
}

int _os_platform_schedule_task(os_task_function function, void *arg, uint16_t start_delay_secs, uint8_t clear_interrupts) {
    cli();
    int ret = start_task(function, arg, start_delay_secs);
    sei();
    return ret;
//    return 0;
}
uint32_t *retAddress = 0;

void _os_platform_switch_tasks() {
    if(num_tasks == 0) return;
    ///printf("switching()\n");

    while(1) {
        cur_task++;
        if(cur_task == num_tasks) cur_task = 0;

        if(tasks[cur_task].running == 1 && tasks[cur_task].done == 0) {
            if(tasks[cur_task].delayMillis <= 0) {
                retAddress = tasks[cur_task].address;
                break;
            }
        }
        else if(tasks[cur_task].running == 0 && tasks[cur_task].delayMillis <= 0 && tasks[cur_task].done != 1) {
            if(tasks[cur_task].original_sp == NULL) {
                tasks[cur_task].original_sp = task_stack_top_ptr;
                tasks[cur_task].saved_sp = task_stack_top_ptr;
                task_stack_top_ptr -= TASK_STACK_SIZE;
            }
            else {
                tasks[cur_task].saved_sp = tasks[cur_task].original_sp;
            }
            memset(tasks[cur_task].original_sp - TASK_STACK_SIZE, 0, TASK_STACK_SIZE);

            retAddress = tasks[cur_task].address;
            tasks[cur_task].running = 1;
            
            break;
        }
    }
}

static uint32_t *sp1;

void  __attribute__((naked)) _os_platform_resume_task()  {
    cli();
    asm volatile(
        "mov sp, %0" : : "r" (sp1)
    );

    asm volatile(
        "mov lr, %0\n" : : "r" (retAddress)
    );
    sei();
    asm volatile(
        "movs pc, lr"
    );
}

void __attribute__((naked)) _os_platform_do_something_else() {
    asm volatile("mov %0, sp" : "=r" (sp1));
    asm volatile("mov %0, lr" : "=r" (retAddress));

    if(cur_task != -1) {
        tasks[cur_task].address = retAddress;
        tasks[cur_task].saved_sp = sp1;
    }
    asm volatile("swi 2");
}

void _os_platform_update_delay_millis() {
    int x;
    for(x = 0; x < num_tasks; x++) {
        if(tasks[x].running == 1 && tasks[x].delayMillis > 0) {
            tasks[x].delayMillis -= TICK_INTERVAL;
        }
    }
}

/**
 * 
   10784:	e52db004 	push	{fp}		; (str fp, [sp, #-4]!)
   10788:	e28db000 	add	fp, sp, #0
   1078c:	e28bd000 	add	sp, fp, #0
   10790:	e49db004 	pop	{fp}		; (ldr fp, [sp], #4)
   10794:	e1b0f00e 	movs	pc, lr
 */
//void __attribute__((interrupt("SWI"))) swi_handler() {
void __attribute__((naked)) swi_handler() {
    

    _os_platform_switch_tasks();
    sp1 = tasks[cur_task].saved_sp;

    asm volatile(
        "mov lr, %0\n" : : "r" (_os_platform_resume_task)
    );
    asm volatile(
        "movs pc, lr"
    );
}

void __attribute__((naked)) interrupt_hdlr() {
    asm volatile("mov %0, lr" : "=r" (retAddress));
    
    uptime_millis += TICK_INTERVAL;

        retAddress -= 4; 
    if(*(TIMER0 + TIMER_MIS)) {
        *(TIMER0 + TIMER_INTCLR) = 1;

    } 
    if(cur_task != -1) {
        tasks[cur_task].address = retAddress;
    }
    _os_platform_update_delay_millis();
    _os_platform_switch_tasks();
    sp1 = tasks[cur_task].saved_sp;


    asm volatile(
        "mov lr, %0\n" : : "r" (_os_platform_resume_task)
    );
    asm volatile(
        "movs pc, lr"
    );
}

#include <sys/stat.h>

int _close(int file) { return -1; }

int _fstat(int file, struct stat *st) {
 st->st_mode = S_IFCHR;
 return 0;
}

int _isatty(int file) { return 1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _open(const char *name, int flags, int mode) { return -1; }

int _read(int file, char *ptr, int len) {
    return -1;
}

char *heap_end = 0;
caddr_t _sbrk(int incr) {
 extern char heap_low; /* Defined by the linker */
 extern char heap_top; /* Defined by the linker */
 char *prev_heap_end;

 if (heap_end == 0) {
  heap_end = &heap_low;
 }
 prev_heap_end = heap_end;

 if (heap_end + incr > &heap_top) {
  /* Heap and stack collision */
  return (caddr_t)0;
 }

 heap_end += incr;
 return (caddr_t) prev_heap_end;
 }

int _write(int file, char *ptr, int len) {
 int todo;

 for (todo = 0; todo < len; todo++) {
  *UART0DR = *ptr++;
 }
 return len;
 }

void _os_platform_spinlock_acquire(spinlock_t *lock) {
    while(1) {
        if(*lock == 0) {
            cli();
            if(*lock == 0) {
                *lock = 1;
                sei();
                return;
            }
            sei();
        }
    }
}

void _os_platform_spinlock_release(spinlock_t *lock) {
    cli();
    *lock = 0;
    sei();
}

void _os_platform_mutex_acquire(mutex_t *mutex) {
    uint8_t pending = 0;

    while(1) {
        cli();
        if(mutex->value == 0 && (mutex->wait == 0 || mutex->wait == cur_task)) {
            mutex->value = 1; 
            mutex->wait = 0;
            sei();
            return;
        }
        else {
            if(pending == 0 && mutex->wait == 0) {
                pending = 1; 
                mutex->wait = cur_task;
            }
            _os_platform_do_something_else();
        }
    }
}

void _os_platform_mutex_release(mutex_t *mutex) {
    mutex->value = 0;
}
