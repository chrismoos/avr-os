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


void kernel_task(void *arg) {
    int x = 0;
    while(1) {
        printf("kernel task running: %d, uptime: %lu, uptime_ms: %lu\n", x++, (long)os_get_uptime(), (long)os_get_uptime_millis());
        os_sleep(1000);
    }
}

void user_task(void *arg) {
    printf("user task!!!\n");

    os_sleep(2000);
    printf("exiting\n");

    os_schedule_task(user_task, NULL, 1);
    os_exit_task();
}

int main() {
    os_init();

    printf("Starting application: %d...\n", 0);

    os_schedule_task(kernel_task, NULL, 0);
    os_schedule_task(user_task, NULL, 1);

    os_loop(); 
    return 0;
}
