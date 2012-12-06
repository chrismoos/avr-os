#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#ifdef SIMAVR
#include <simavr/avr/avr_mcu_section.h>
#endif

#include <util/delay.h>

#include <stdio.h>
#include <string.h>

#include "../os.h"
#include "../os_internal.h"

#include "../context.h"

#ifdef SIMAVR
AVR_MCU_SIMAVR_CONSOLE(&GPIOR0);
#endif

static void *tempSp;
void *stackTop, *tasksStackTop;

#ifdef SIMAVR
static int uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
		uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                         _FDEV_SETUP_WRITE);
#endif

void _os_platform_init() {
#ifdef SIMAVR
    GPIOR0 = SIMAVR_CMD_UART_LOOPBACK;
    stdout = &mystdout;
#endif

    cli();

    // clear on compare match
    TCCR1A = _BV(COM1A1);

    // clkio/256, CTC, TOP = OCR1A
    TCCR1B = _BV(CS12) | _BV(WGM12);

    OCR1A = F_CPU / 256 / (1000 / TICK_INTERVAL);

    TIMSK1 = _BV(OCIE1A);
}

void _os_platform_loop() {
    asm volatile("in r0, __SP_L__\nsts stackTop, r0\nin r0, __SP_H__\nsts stackTop+1, r0");

    // reserve for the while loop
    stackTop -= 32;
    tasksStackTop = stackTop - 256;

    sei();

    while(1)
        sleep_cpu();
}

void _os_platform_sleep() {
    sleep_cpu();
}

int _os_platform_schedule_task(os_task_function function, void *arg, uint16_t start_delay_secs, uint8_t clear_interrupts) {
    if(clear_interrupts == 1 && (SREG & (1 << 7)) == (1 << 7)) {
        cli();
        int ret = start_task(function, arg, start_delay_secs);
        
        sei();
        return ret;
    }
    else {
        return start_task(function, arg, start_delay_secs);
    }
}

void _os_platform_switch_tasks() {
    if(num_tasks == 0) return;


    while(1) {
        cur_task++;
        if(cur_task == num_tasks) cur_task = 0;

        if(tasks[cur_task].running == 1) {
            if(tasks[cur_task].delayMillis <= 0) {
                tempSp = tasks[cur_task].saved_sp;
                break;
            }
        }
        else if(tasks[cur_task].done == 1) {
            continue;
        }
        else if(tasks[cur_task].start_delay_secs == 0) {
            uint8_t *top;

            if(tasks[cur_task].original_sp != NULL) {
                top = tasks[cur_task].original_sp; 
            }
            else {
                top = tasksStackTop;
                tasksStackTop -= TASK_STACK_SIZE;
                tasks[cur_task].original_sp = top;
            }
            memset(top - TASK_STACK_SIZE, 0, TASK_STACK_SIZE);

            *(top) = ((uint16_t)tasks[cur_task].address);
            *(top - 1) = (((uint16_t)tasks[cur_task].address) >> 8);

            uint8_t *registerStartAddr = top - 3;
            *(registerStartAddr - 24) = ((uint16_t)tasks[cur_task].arg);
            *(registerStartAddr - 25) = (((uint16_t)tasks[cur_task].arg) >> 8);

            tasks[cur_task].saved_sp = top - 35;
            tasks[cur_task].running = 1;
            tempSp = tasks[cur_task].saved_sp;

            break;
        }
    }

    
}

void _os_platform_do_something_else() {
    cli();

    SAVE_CONTEXT(tempSp)

    // set the stack top for the ISR
    asm volatile(
        "lds r16, stackTop\n"
        "lds r17, stackTop+1\n"
        "out __SP_L__, r16\n"
        "out __SP_H__, r17\n"
    );

    if(cur_task != -1) {
        tasks[cur_task].saved_sp = tempSp;
    }

    _os_platform_switch_tasks();

    RESTORE_CONTEXT(tempSp)
    sei();
}

void _os_platform_update_delay_millis() {
    int x;
    for(x = 0; x < num_tasks; x++) {
        if(tasks[x].running == 1 && tasks[x].delayMillis > 0) {
            tasks[x].delayMillis -= TICK_INTERVAL;
        }
    }
}

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
    SAVE_CONTEXT(tempSp)

    // set the stack top for the ISR
    asm volatile(
        "lds r16, stackTop\n"
        "lds r17, stackTop+1\n"
        "out __SP_L__, r16\n"
        "out __SP_H__, r17\n"
    );

    uptime_millis += TICK_INTERVAL;

    if(cur_task != -1) {
        tasks[cur_task].saved_sp = tempSp;
    }


    _os_platform_switch_tasks();
    _os_platform_update_delay_millis();

    RESTORE_CONTEXT(tempSp)
    asm volatile("reti");
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
