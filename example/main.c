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
#include "os.h"

mutex_t mutex;

void kernel_task(void *arg) {
    int x = 0;
    while(1) {
        mutex_acquire(&mutex);
        printf("kernel task running: %d, uptime: %lu, uptime_ms: %lu\n", x++, (long)os_get_uptime(), (long)os_get_uptime_millis());
        mutex_release(&mutex);
        os_sleep(1000);
    }
}

void user_task(void *arg) {
    int x = 0;
    for(x = 0; x < 2; x++) {
        mutex_acquire(&mutex);
        printf("user task: %lu, n: %d\n", (long)os_get_uptime_millis(), x);
        mutex_release(&mutex);

        os_sleep(1000);
    }

    os_schedule_task(user_task, NULL, 1);
    os_exit_task();
}

int main() {
    os_init();

    mutex_init(&mutex);

    printf("Starting application: %d...\n", 0);

    os_schedule_task(kernel_task, NULL, 0);
    os_schedule_task(user_task, NULL, 1);

    os_loop(); 
    return 0;
}
