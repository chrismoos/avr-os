ifeq ($(ARM_HOME),)
$(error ARM_HOME must be defined.)
endif

TARGET_OS_TASK_STACK_SIZE := 2048
TARGET_OS_TICK_INTERVAL_MS := 10
TARGET_OS_MAX_TASKS := 5

AS_FLAGS += -mcpu=arm1176jz-s
CFLAGS += -nostdlib -fno-builtin -mcpu=arm1176jz-s

CC := $(ARM_HOME)/bin/arm-none-eabi-gcc
AR := $(ARM_HOME)/bin/arm-none-eabi-ar

CFLAGS += -DMAX_TASKS=$(TARGET_OS_MAX_TASKS)
CFLAGS += -DTASK_STACK_SIZE=$(TARGET_OS_TASK_STACK_SIZE)
CFLAGS += -DTICK_INTERVAL=$(TARGET_OS_TICK_INTERVAL_MS)

TARGET_AVR_OS_OUT = build/avr-os.a
TARGET_AVR_EXAMPLE_OUT = build/avr-os-example.img

os: build-dir
	@echo -e "$(ccgreen)Compiling OS sources...$(ccend)"
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c platform/arm11.c -o build/arm11.o
	$(ARM_HOME)/bin/arm-none-eabi-as $(AS_FLAGS) -o build/start.o platform/arm11.s
	$(ARM_HOME)/bin/arm-none-eabi-as $(AS_FLAGS) -o build/vectors.o platform/arm11_vectors.s
	rm -f $(TARGET_AVR_OS_OUT)
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/os.o
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/arm11.o
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/vectors.o

example: os
	@echo -e "$(ccgreen)Creating example...$(ccend)"
	$(CC) $(CFLAGS) -I. -c example/main.c -o build/main.o
	$(CC) -T platform/arm11_link.ld build/main.o $(TARGET_AVR_OS_OUT) -o build/program
	$(ARM_HOME)/bin/arm-none-eabi-objcopy -O binary build/program $(TARGET_AVR_EXAMPLE_OUT)

run: example
	qemu-system-arm -M versatilepb -nographic -kernel $(TARGET_AVR_EXAMPLE_OUT)

test: os
	$(error "not defined yet")

.PHONY: _os _example _run _test
