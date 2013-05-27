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

#define assert(expr) if(!(expr)) { printf("assertion failed: %s\n", #expr); return -1; }

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#define TEST(fn_name) run_test(#fn_name, fn_name);
int failed_tests = 0;
int passed_tests = 0;

typedef int (testFn)(void);

void run_test(char *name, testFn fn) {
    printf("%s: \n", name);
    if(fn() == 0) {
        passed_tests += 1;
        printf("pass\n");
    }
    else {
        failed_tests += 1;
        printf("fail\n");
    }
}

int test_nothing() {
    assert(1 == 1);
    return 0;
}

void test_task(void *arg) {
    TEST(test_nothing);

    printf("test summary: %d passed, %d failed\n", passed_tests, failed_tests);
    if(failed_tests > 0) {
        printf("TEST FAILURE\n");
    }
    else {
        printf("TESTS PASSED\n");
    }

    cli();
    sleep_mode();
}

int main() {
    os_init();

    os_schedule_task(test_task, NULL, 0);

    os_loop();
    return 0;
}
