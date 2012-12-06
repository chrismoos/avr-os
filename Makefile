-include Makefile.common

OS_SOURCES=os.c
OS_OBJECTS=build/os.o

APP_SOURCES=main.c
APP_OBJECTS=build/main.o

ifeq ($(PLATFORM), arm)

ARM_HOME := /Users/chrismoos/arm-cs-tools
CC := $(ARM_HOME)/bin/arm-none-eabi-gcc

TARGET_OS_TASK_STACK_SIZE := 2048
TARGET_OS_TICK_INTERVAL_MS := 10
TARGET_OS_MAX_TASKS := 8

OS_SOURCES += platform/arm11.c

AS_FLAGS += -mcpu=arm1176jz-s
CFLAGS += -nostdlib -fno-builtin -mcpu=arm1176jz-s

else

AVR_HOME := /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr
CC := $(AVR_HOME)/bin/avr-gcc
CFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \
	-DF_CPU=16000000 -I $(AVR_HOME)/include  -I/usr/local/include -O1

CFLAGS += -DSIMAVR

OS_SOURCES+=platform/avr.c

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

assembly:
	mkdir -p build
	$(CC) $(CFLAGS) -S -o build/main.s -c main.c
	$(CC) $(CFLAGS) -S -o build/os.s -c os.c

ifeq ($(PLATFORM), arm)
os: build
	$(ARM_HOME)/bin/arm-none-eabi-as $(AS_FLAGS) -o build/start.o platform/arm11.s
	$(ARM_HOME)/bin/arm-none-eabi-as $(AS_FLAGS) -o build/vectors.o platform/arm11_vectors.s
	$(CC) $(CFLAGS) -c main.c -o build/main.o
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c platform/arm11.c -o build/arm11.o
	$(ARM_HOME)/bin/arm-none-eabi-gcc -T platform/arm11_link.ld build/main.o build/arm11.o build/os.o build/start.o build/vectors.o -o build/program
	$(ARM_HOME)/bin/arm-none-eabi-objcopy -O binary build/program build/program.img
else
os: build
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c platform/avr.c -o build/avr.o
	$(CC) $(CFLAGS) -c main.c -o build/main.o
	$(CC) $(CFLAGS) build/os.o build/avr.o build/main.o ~/Storage/opensource/hello-avr-os/.lib/arduino.a -o build/program
endif

app: build
	$(CC) $(CFLAGS) -c $(APP_SOURCES) -o $(APP_OBJECTS)

run: all
	simavr -m $(TARGET_MMCU) -v -f 16000000 build/program

all: os
