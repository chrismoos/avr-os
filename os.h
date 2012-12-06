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
