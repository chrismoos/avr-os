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

/**
 * Schedules a task for execution.
 *
 * @param function The function that will execute when the task is started.
 * @param arg A user defined value that will be passed to the task.
 * @param start_delay_secs The amount of seconds before starting this task or
 * 0 if the task should be started immediately.
 */
int os_schedule_task(os_task_function function, void *arg, uint16_t start_delay_secs);

/**
 * This MUST be called at the end of a task.
 */
void os_exit_task();

/**
 * Switches to the next available task.
 */
void os_yield();

/**
 * Suspends the current task for the specified time.
 */
void os_sleep(uint16_t millis);

/**
 * Returns the amount of seconds since the OS has started.
 */
uint32_t os_get_uptime();

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

struct mutex_s {
    // This is set to 1 if the mutex is currently held or 0 if available.
    uint8_t value;

    // This is set to the next task waiting for the mutex.
    // A queue could be used but this should give a good balance of fairness
    // and performance.
    //
    // NOTE: Tasks must always release their mutexes before exiting or a deadlock 
    // may occur.
    uint8_t wait;
};
typedef volatile struct mutex_s mutex_t;

/**
 * Initialize a mutex. 
 *
 * After initializiation the mutex is available to acquire.
 */
void mutex_init(mutex_t *mutex);

/**
 * Acquires the specified mutex.
 *
 * If the mutex is being held by another task this call 
 * will block and another task will be executed.
 *
 * When this function returns, the mutex is held by the current task.
 */
void mutex_acquire(mutex_t *mutex);

/**
 * Releases the specified mutex.
 *
 * This should only be called after a call to mutex_acquire();
 * In addition, calls to acquire/release should be balanced.
 */
void mutex_release(mutex_t *mutex);
