ifeq ($(AVR_HOME),)
AVR_HOME := /Applications/Arduino.app/Contents/Resources/Java
endif

ifeq ($(AVR_INC),)
AVR_INC := $(shell locate avr/include | head -1)
endif

ifeq ($(ARDUINO_INC),)
ARDUINO_INC := $(shell locate Arduino.h | head -1 | sed "s/\/Arduino.h//")
endif

ifeq ($(CONFIG_AVR_TIMER),)
CONFIG_AVR_TIMER := 1
endif

ifeq (,$(filter $(CONFIG_AVR_TIMER), 0 1 2))
$(error Only TIMER 0, 1, and 2 are supported for CONFIG_AVR_TIMER)
endif

CC := $(shell locate avr-gcc | head -1)
LD := $(shell locate avr-ld | head -1)
AR := $(shell locate avr-ar | head -1)
CFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \
	-DF_CPU=16000000 -I $(AVR_HOME)/hardware/tools/avr/lib/avr/include \
	-I$(AVR_INC) -I$(ARDUINO_INC) -I/usr/local/include -O1 -Wall
CFLAGS += -mmcu=$(TARGET_MMCU)

ifeq ($(CONFIG_SIMAVR),1)
CFLAGS += -DSIMAVR
endif

CFLAGS += -DMAX_TASKS=$(TARGET_OS_MAX_TASKS)
CFLAGS += -DTASK_STACK_SIZE=$(TARGET_OS_TASK_STACK_SIZE)
CFLAGS += -DTICK_INTERVAL=$(TARGET_OS_TICK_INTERVAL_MS)

CFLAGS += -DCONFIG_AVR_TIMER=$(CONFIG_AVR_TIMER)

TARGET_AVR_OS_OUT = build/avr-os.a
TARGET_AVR_EXAMPLE_OUT = build/avr-os-example.img

os: build-dir
	@echo "Compiling OS sources..."
	$(CC) $(CFLAGS) -c os.c -o build/os.o
	$(CC) $(CFLAGS) -c utility/avr.c -o build/avr.o
	rm -f $(TARGET_AVR_OS_OUT)
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/os.o
	$(AR) rcs $(TARGET_AVR_OS_OUT) build/avr.o 

example: os
	@echo "Creating example..."
	$(CC) $(CFLAGS) -I. -c example/main.c -o build/main.o
	$(CC) $(CFLAGS) build/main.o $(TARGET_AVR_OS_OUT) -o $(TARGET_AVR_EXAMPLE_OUT)

run: example
	simavr -m $(TARGET_MMCU) -v -f 16000000 $(TARGET_AVR_EXAMPLE_OUT)

test: os
	$(CC) $(CFLAGS) -I. -c test/test.c -o build/test.o
	$(CC) $(CFLAGS) build/test.o $(TARGET_AVR_OS_OUT) -o build/test
	simavr -m $(TARGET_MMCU) -v -f 16000000 build/test

.PHONY: os example run test
