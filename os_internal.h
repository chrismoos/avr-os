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

#ifndef MAX_TASKS
#ifdef ARDUINO

#define MAX_TASKS 5
#define TICK_INTERVAL 10
#define TASK_STACK_SIZE 256

#endif
#endif

#define TASK_FLAG_RUNNING 1
#define TASK_FLAG_DONE 2

#define BV(bit) (1 << bit)
#define BIT_CLEAR(n, bit) n &= ~BV(bit);
#define BIT_SET(n, bit) n |= BV(bit);
#define BIT_ISSET(n, bit) (n & BV(bit))

struct task_definition {
    void *arg;
    void *address;
    uint8_t flags;
    void *original_sp;
    void *saved_sp;
    uint32_t start_delay_secs;
    uint32_t delayMillis;
};
typedef struct task_definition task_definition;

int start_task(void *addr, void *arg, uint16_t start_delay);

extern task_definition tasks[MAX_TASKS];

extern volatile int cur_task;
extern volatile int num_tasks;

extern uint32_t uptime_secs;
extern uint64_t uptime_millis;
