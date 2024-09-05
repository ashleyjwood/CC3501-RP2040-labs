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