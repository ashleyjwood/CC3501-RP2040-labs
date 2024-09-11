#include "drivers/led.h"
#include "drivers/colors.h"
#include "pico/rand.h"
#include <cmath>
#include <map>
#include <string>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#define PICO_LED_PIN 14
#define NUMBER_OF_LEDS 12
#define ON_TIME 50
#define OFF_TIME 50
#define test_led_1 4
#define test_led_2 5

int main()
{
    stdio_init_all();
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    LED led(PICO_LED_PIN, pio, 0, offset, NUMBER_OF_LEDS);


}