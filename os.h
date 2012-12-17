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

#include <stdint.h>


typedef void(*os_task_function)(void *arg);

/**
 * Initializes the OS
 *
 * This should be the first call in the
 * application's main method.
 */
void os_init();

/**
 * Starts the OS loop.
 *
 * This will execute tasks.
 * Make sure to create at least one task before 
 * starting the loop.
 */
void os_loop();

int os_schedule_task(os_task_function function, void *arg, uint16_t start_delay_secs);

void os_exit_task();

void os_sleep(uint16_t millis);

/**
 * Returns the amount of seconds since the OS has started.
 */
uint64_t os_get_uptime();

/**
 * Returns the amount of milliseconds since the OS has started.
 *
 * This will overflow after about 
 */
uint64_t os_get_uptime_millis();

typedef volatile int spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

struct mutex_t {
    uint8_t value;
    uint8_t wait;
};
typedef volatile struct mutex_t mutex_t;

void mutex_init(mutex_t *mutex);
void mutex_acquire(mutex_t *mutex);
void mutex_release(mutex_t *mutex);
