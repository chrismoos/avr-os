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

#define xstr(s) str(s)
#define str(s) #s

#define PRINT_STACK_POINTER \
    asm volatile("push r0\nin r0, __SP_L__\nsts stackPointer, r0\nin r0, __SP_H__\nsts stackPointer+1, r0\npop r0"); printf("stack pointer: %04x\n", stackPointer);

#define SAVE_CONTEXT(sp) \
    asm volatile( \
        "push r0\n" \
        "in r0, __SREG__\n" \
        "cli\n" \
        "push r0\n" \
        "push r1\n" \
        "push r2\n" \
        "push r3\n" \
        "push r4\n" \
        "push r5\n" \
        "push r6\n" \
        "push r7\n" \
        "push r8\n" \
        "push r9\n" \
        "push r10\n" \
        "push r11\n" \
        "push r12\n" \
        "push r13\n" \
        "push r14\n" \
        "push r15\n" \
        "push r16\n" \
        "push r17\n" \
        "push r18\n" \
        "push r19\n" \
        "push r20\n" \
        "push r21\n" \
        "push r22\n" \
        "push r23\n" \
        "push r24\n" \
        "push r25\n" \
        "push r26\n" \
        "push r27\n" \
        "push r28\n" \
        "push r29\n" \
        "push r30\n" \
        "push r31\n" \
        "in r0, __SP_L__\n" \
        "sts "xstr(sp)", r0\n" \
        "in r0, __SP_H__\n" \
        "sts "xstr(sp+1)", r0\n" \
    );

#define RESTORE_CONTEXT(sp) \
    asm volatile( \
        "lds r30, "xstr(sp)"\n" \
        "lds r31, "xstr(sp+1)"\n" \
        "out __SP_L__, r30\n" \
        "out __SP_H__, r31\n" \
        "pop r31\n" \
        "pop r30\n" \
        "pop r29\n" \
        "pop r28\n" \
        "pop r27\n" \
        "pop r26\n" \
        "pop r25\n" \
        "pop r24\n" \
        "pop r23\n" \
        "pop r22\n" \
        "pop r21\n" \
        "pop r20\n" \
        "pop r19\n" \
        "pop r18\n" \
        "pop r17\n" \
        "pop r16\n" \
        "pop r15\n" \
        "pop r14\n" \
        "pop r13\n" \
        "pop r12\n" \
        "pop r11\n" \
        "pop r10\n" \
        "pop r9\n" \
        "pop r8\n" \
        "pop r7\n" \
        "pop r6\n" \
        "pop r5\n" \
        "pop r4\n" \
        "pop r3\n" \
        "pop r2\n" \
        "pop r1\n" \
        "pop r0\n" \
        "out __SREG__, r0\n" \
        "pop r0\n" \
    );
