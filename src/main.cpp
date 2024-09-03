#include "drivers/led.h"
#include "drivers/colors.h"

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

int main()
{
    stdio_init_all();

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    LED led(PICO_LED_PIN, pio, 0, offset);

    for (;;)
    {

        for (int i = 0; i < 12; ++i)
        {
            // Loop red
            led.turn_led_on(i, RED);
            led.update_led();
            sleep_ms(ON_TIME);
        }
        for (int i = 0; i < 12; ++i)
        {
            // Loop green
            led.turn_led_on(i, GREEN);
            led.update_led();
            sleep_ms(ON_TIME);
        }
        for (int i = 0; i < 12; ++i)
        {
            // Loop blue
            led.turn_led_on(i, BLUE);
            led.update_led();
            sleep_ms(ON_TIME);
        }
        for (int i = 0; i < 12; ++i)
        {
            // Loop off
            led.turn_led_off(i);
            led.update_led();
            sleep_ms(OFF_TIME);
        }
        for (int i = 0; i < 12; ++i)
        {
            led.turn_led_on(i, RED);
        }
        led.update_led();
        sleep_ms(250);

        // Set all LEDs to green
        for (int i = 0; i < 12; ++i)
        {
            led.turn_led_on(i, GREEN);
        }
        led.update_led();
        sleep_ms(250);

        // Set all LEDs to blue
        for (int i = 0; i < 12; ++i)
        {
            led.turn_led_on(i, BLUE);
        }
        led.update_led();
        sleep_ms(250);
    }

    return 0;
}
