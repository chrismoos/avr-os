-include Makefile.common

ifeq ($(PLATFORM), arm)

ifeq ($(ARM_HOME),)
$(error ARM_HOME must be defined.)
endif

CC := $(ARM_HOME)/bin/arm-none-eabi-gcc

TARGET_OS_TASK_STACK_SIZE := 2048
TARGET_OS_TICK_INTERVAL_MS := 10
TARGET_OS_MAX_TASKS := 8

AS_FLAGS += -mcpu=arm1176jz-s
CFLAGS += -nostdlib -fno-builtin -mcpu=arm1176jz-s

else

ifeq ($(AVR_HOME),)
AVR_HOME := /Applications/Arduino.app/Contents/Resources/Java
endif

CC := $(AVR_HOME)/hardware/tools/avr/bin/avr-gcc
CFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \
	-DF_CPU=16000000 -I $(AVR_HOME)/hardware/tools/avr/lib/avr/include  -I/usr/local/include -O1 -Wall

CFLAGS += -DSIMAVR

ifeq ($(DEVICE),atmega1280)
	TARGET_MMCU := atmega1280
	TARGET_OS_TASK_STACK_SIZE := 512
	TARGET_OS_TICK_INTERVAL_MS := 10
	TARGET_OS_MAX_TASKS := 18
else
	TARGET_MMCU := atmega328p
	TARGET_OS_TASK_STACK_SIZE := 256
	TARGET_OS_TICK_INTERVAL_MS := 10
	TARGET_OS_MAX_TASKS := 5
endif

CFLAGS += -mmcu=$(TARGET_MMCU)

endif


CFLAGS += -DMAX_TASKS=$(TARGET_OS_MAX_TASKS)
CFLAGS += -DTASK_STACK_SIZE=$(TARGET_OS_TASK_STACK_SIZE)
CFLAGS += -DTICK_INTERVAL=$(TARGET_OS_TICK_INTERVAL_MS)


default: all

clean:
	rm -rf build

build:
	mkdir -p build

ifeq ($(PLATFORM), arm)
os: build
	$(ARM_HOME)/bin/arm-none-eabi-as $(AS_FLAGS) -o build/start.o platform/arm11.s
	$(ARM_HOME)/bin/arm-none-eabi-as $(AS_FLAGS) -o build/vectors.o platform/arm11_vectors.s
	$(CC) $(CFLAGS) -c -I. example/main.c -o build/main.o
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c platform/arm11.c -o build/arm11.o
	$(ARM_HOME)/bin/arm-none-eabi-gcc -T platform/arm11_link.ld build/main.o build/arm11.o build/os.o build/start.o build/vectors.o -o build/program
	$(ARM_HOME)/bin/arm-none-eabi-objcopy -O binary build/program build/program.img

run: all
	qemu-system-arm -M versatilepb -kernel build/program.img -nographic

else
test: os
	$(CC) $(CFLAGS) -I. -c test/test.c -o build/test.o 
	$(CC) $(CFLAGS) build/os.o build/avr.o build/test.o -o build/test
	simavr -m $(TARGET_MMCU) -v -f 16000000 build/test

os: build
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c utility/avr.c -o build/avr.o
	$(CC) $(CFLAGS) -I. -c example/main.c -o build/main.o
	$(CC) $(CFLAGS) build/os.o build/avr.o build/main.o -o build/program

run: all
	simavr -m $(TARGET_MMCU) -v -f 16000000 build/program

endif

all: os
