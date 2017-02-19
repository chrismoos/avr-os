include common.mk

ifeq ($(DEVICE),)
	DEVICE_NO := $(shell read -p "Select device 1.UNO 2.MEGA 3. MEGA2560 : "; echo $$REPLY)
	
	DEVICE := arduino_uno
	
	ifeq ($(DEVICE_NO),2)
		DEVICE := arduino_mega
	endif

	ifeq ($(DEVICE_NO),3)
		DEVICE := arduino_mega2560
	endif
endif

DEVICE_VENDOR := $(firstword $(subst _, ,$(DEVICE)))
DEVICE_NAME := $(word 2,$(subst _, ,$(DEVICE)))

summary = \
	echo "\n$(ccgreen)avr-os build summary:$(ccend)\n"; \
	echo "$(ccyellow)OS static library:$(ccend) $(TARGET_AVR_OS_OUT)"; \
	echo "$(ccyellow)Example image:$(ccend) $(TARGET_AVR_EXAMPLE_OUT)";

all: example
	$(summary)

include device/$(DEVICE_VENDOR)/$(DEVICE_NAME)/device.mk

build-dir:
	mkdir -p build

.PHONY: clean
clean:
	rm -rf build
