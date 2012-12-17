#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "os.h"

int main(int argc, char **argv) {
    cli();
    sleep_mode();
    return 0;
}
