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
