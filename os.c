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

#include <stdio.h>
#include <string.h>
#include "os.h"
#include "os_internal.h"


extern void _os_platform_init();
extern void _os_platform_loop();
extern void _os_platform_sleep();
extern int _os_platform_schedule_task(os_task_function function, void *arg, uint16_t start_delay_secs, uint8_t clear_interrupts);
extern void _os_platform_switch_tasks();
extern void _os_platform_do_something_else();

extern void _os_platform_spinlock_acquire(spinlock_t *lock);
extern void _os_platform_spinlock_release(spinlock_t *lock);

extern void _os_platform_mutex_acquire(mutex_t *mutex);
extern void _os_platform_mutex_release(mutex_t *mutex);


extern int main();

task_definition tasks[MAX_TASKS];

volatile int cur_task = -1;
volatile int num_tasks = 0;

uint64_t uptime_secs = 0;
uint64_t uptime_millis = 0;

int start_task(void *addr, void *arg, uint16_t start_delay) {
    if(num_tasks == MAX_TASKS) {
        // search for a task that is marked as done, take it over
        int x;
        for(x = 0; x < num_tasks; x++) {
            if(tasks[x].done == 1) {
                tasks[x].address = addr;
                tasks[x].arg = arg;
                tasks[x].start_delay_secs = start_delay;
                tasks[x].done = 0;
                tasks[x].running = 0;
                tasks[x].saved_sp = NULL;
                tasks[x].delayMillis = 0;
                return 0;
            }
        }
        return -1;
    }
    else {
        tasks[num_tasks].address = addr;
        tasks[num_tasks].arg = arg;
        tasks[num_tasks].start_delay_secs = start_delay;
        num_tasks++;
        return 0;
    }
}

int os_schedule_task(os_task_function function, void *arg, uint16_t start_delay_secs) {
   return _os_platform_schedule_task(function, arg, start_delay_secs, 1);
}

void do_something_else() {
    _os_platform_do_something_else();
}

void os_sleep(uint16_t millis) {
    tasks[cur_task].delayMillis = millis - TICK_INTERVAL;
    do_something_else();
}

void _os_hangout(void *arg) {
    while(1) {
        _os_platform_sleep();
    }
}

void _os_task_delay_starter(void *arg) {
    int x;
    while(1) {
        os_sleep(1000);
        uptime_secs++;

        for(x = 0; x < num_tasks; x++) {
            if(tasks[x].running == 0 && tasks[x].done != 1 &&
                tasks[x].start_delay_secs > 0) {

                tasks[x].start_delay_secs -= 1;

            }
        }
    }
}

void os_init() {
    _os_platform_init();
    
    _os_platform_schedule_task(_os_hangout, NULL, 0, 0);
    _os_platform_schedule_task(_os_task_delay_starter, NULL, 0, 0);
}

void os_exit_task() {
    tasks[cur_task].running = 0;
    tasks[cur_task].done = 1;

    do_something_else();
}

uint64_t os_get_uptime() {
    return uptime_secs;
}

uint64_t os_get_uptime_millis() {
    return uptime_millis;
}

void os_loop() {
    _os_platform_loop();
}


void spinlock_init(spinlock_t *lock) {
    *lock = 0;
}

void spinlock_acquire(spinlock_t *lock) {
    _os_platform_spinlock_acquire(lock);
}

void spinlock_release(spinlock_t *lock) {
    _os_platform_spinlock_release(lock); 
}

void mutex_init(mutex_t *mutex) {
    memset((void*)mutex, 0, sizeof(mutex_t));
}

void mutex_acquire(mutex_t *mutex) {
    _os_platform_mutex_acquire(mutex);
}

void mutex_release(mutex_t *mutex) {
    _os_platform_mutex_release(mutex);
}
