ifeq ($(AVR_HOME),)
AVR_HOME := /Applications/Arduino.app/Contents/Resources/Java
endif

CC := $(AVR_HOME)/hardware/tools/avr/bin/avr-gcc
LD := $(AVR_HOME)/hardware/tools/avr/bin/avr-ld
AR := $(AVR_HOME)/hardware/tools/avr/bin/avr-ar
CFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \
	-DF_CPU=16000000 -I $(AVR_HOME)/hardware/tools/avr/lib/avr/include  -I/usr/local/include -O1 -Wall
CFLAGS += -mmcu=$(TARGET_MMCU)

ifeq ($(CONFIG_SIMAVR),1)
CFLAGS += -DSIMAVR
endif

CFLAGS += -DMAX_TASKS=$(TARGET_OS_MAX_TASKS)
CFLAGS += -DTASK_STACK_SIZE=$(TARGET_OS_TASK_STACK_SIZE)
CFLAGS += -DTICK_INTERVAL=$(TARGET_OS_TICK_INTERVAL_MS)

TARGET_AVR_OS_OUT = build/avr-os.a
TARGET_AVR_EXAMPLE_OUT = build/avr-os-example.img

os: build-dir
	@echo -e "$(ccgreen)Compiling OS sources...$(ccend)"
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c utility/avr.c -o build/avr.o
	rm -f $(TARGET_AVR_OS_OUT)
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/os.o
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/avr.o 

example: os
	@echo -e "$(ccgreen)Creating example...$(ccend)"
	$(CC) $(CFLAGS) -I. -c example/main.c -o build/main.o
	$(CC) $(CFLAGS) build/main.o $(TARGET_AVR_OS_OUT) -o $(TARGET_AVR_EXAMPLE_OUT)

run: example
	simavr -m $(TARGET_MMCU) -v -f 16000000 $(TARGET_AVR_EXAMPLE_OUT)

test: os
	$(CC) $(CFLAGS) -I. -c test/test.c -o build/test.o 
	$(CC) $(CFLAGS) $(TARGET_AVR_OS_OUT) build/test.o -o build/test
	simavr -m $(TARGET_MMCU) -v -f 16000000 build/test

.PHONY: os example run test
