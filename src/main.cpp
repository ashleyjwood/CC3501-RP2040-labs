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

        // for (int i = 0; i < 12; ++i)
        // {
        //     // Loop red
        //     led.turn_led_on(i, RED);
        //     led.update_led();
        //     sleep_ms(ON_TIME);
        // }
        // for (int i = 0; i < 12; ++i)
        // {
        //     // Loop green
        //     led.turn_led_on(i, GREEN);
        //     led.update_led();
        //     sleep_ms(ON_TIME);
        // }
        // for (int i = 0; i < 12; ++i)
        // {
        //     // Loop blue
        //     led.turn_led_on(i, BLUE);
        //     led.update_led();
        //     sleep_ms(ON_TIME);
        // }
        // for (int i = 0; i < 12; ++i)
        // {
        //     // Loop off
        //     led.turn_led_off(i);
        //     led.update_led();
        //     sleep_ms(OFF_TIME);
        // }
        // for (int i = 0; i < 12; ++i)
        // {
        //     led.turn_led_on(i, RED);
        // }
        // led.update_led();
        // sleep_ms(250);

        // // Set all LEDs to green
        // for (int i = 0; i < 12; ++i)
        // {
        //     led.turn_led_on(i, GREEN);
        // }
        // led.update_led();
        // sleep_ms(250);

        // // Set all LEDs to blue
        // for (int i = 0; i < 12; ++i)
        // {
        //     led.turn_led_on(i, BLUE);
        // }
        // led.update_led();
        // sleep_ms(250);

        // led.turn_led_off_all();
        // led.update_led();
        // sleep_ms(250);

        // Initialize the LED colors
        // led.set_multiple_leds(0, CYAN);
        // // led.set_multiple_leds(3, MAGENTA, 4, MAGENTA, 5, MAGENTA);
        // // led.set_multiple_leds(6, YELLOW, 7, YELLOW, 8, YELLOW);
        // // led.set_multiple_leds(9, PURPLE, 10, PURPLE, 11, PURPLE);
        // led.update_led();
        // sleep_ms(100);

        // Shift the CYAN color to the right across the LEDs
        // led.set_multiple_leds(0, RED);
        // for (int i = 0; i < NUMBER_OF_LEDS - 1; ++i) // Adjust the loop count as needed
        // {
        //     led.shift_led_colors_right(i); // Shift colors right by one LED
        //     sleep_ms(250);
        // }

        // sleep_ms(100); // Pause before shifting back

        // // Shift the CYAN color back to the left across the LEDs
        // for (int i = NUMBER_OF_LEDS - 1; i > 0; --i) // Adjust the loop count as needed
        // {
        //     led.shift_led_colors_left(i); // Shift colors left by one LED
        //     sleep_ms(250);
        // }
        // led.turn_led_off_all();
        // sleep_ms(250);

        led.set_multiple_leds(0, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(1, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(2, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(3, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(4, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(5, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(6, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(7, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(8, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(9, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(10, RED);
        busy_wait_ms(100);
        led.set_multiple_leds(11, RED);
        busy_wait_ms(100);
        led.turn_led_off_all();
        busy_wait_ms(100);

    }

    return 0;
}