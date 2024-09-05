#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#define LED_PIN 14
#define NUMBER_OF_LEDS 12 // Number of LEDs in the chain
#define SERVO_PIN 12
#define ON_TIME 2
#define OFF_TIME (20 - ON_TIME) // Total time for each cycle is 20ms 

int main()
{
    stdio_init_all();

    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
    uint32_t led_data[NUMBER_OF_LEDS]; // Array to store data for all LEDs

    gpio_init(SERVO_PIN);
    gpio_set_dir(SERVO_PIN, GPIO_OUT);

    for (;;)
    {
        // Test the log system
        log(LogLevel::INFORMATION, "Hello world");

        // Set all LEDs to the same colour (cyan in this example)
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 25;
        uint32_t color = (red << 24) | (green << 16) | (blue << 8);

<<<<<<< HEAD
        for (int i = 0; i < NUMBER_OF_LEDS; i++)
        {
            led_data[i] = color;
        }

        gpio_put(SERVO_PIN, 1);
        // Send data for all LEDs in the chain
        for (int i = 0; i < NUMBER_OF_LEDS; i++)
        {
            pio_sm_put_blocking(pio0, 0, led_data[i]);
        }
        sleep_ms(ON_TIME);
=======
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
>>>>>>> 6e307c2e51207220662766b733acb24bf5b4c22a

        gpio_put(SERVO_PIN, 0);
        // Turn off all LEDs
        for (int i = 0; i < NUMBER_OF_LEDS; i++)
        {
            led_data[i] = 0;
            pio_sm_put_blocking(pio0, 0, led_data[i]);
        }
        sleep_ms(OFF_TIME);
    }

    return 0;
}