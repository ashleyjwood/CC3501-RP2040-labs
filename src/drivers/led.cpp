#include "led.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include <cstdarg> // For variadic functions
#include <cstdio>

LED::LED(uint ledPin, PIO pioInstance, uint sm, uint offset)
    : ledPin(ledPin), pio(pioInstance), stateMachine(sm), programOffset(offset)
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        led_data[i] = 0; // Initialize all LEDs to off
    }
    ws2812_program_init(pio, stateMachine, programOffset, ledPin, 800000, false);
}

void LED::turn_led_on(uint led_index, uint32_t color)
{
    if (led_index < NUMBER_OF_LEDS)
    {
        led_data[led_index] = color;
    }
}

void LED::turn_led_off(uint led_index)
{
    if (led_index < NUMBER_OF_LEDS)
    {
        led_data[led_index] = 0; // Turn off the specified LED
    }
}

void LED::turn_led_off_all()
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        led_data[i] = 0; // Turn off all LEDs
    }
}

void LED::update_led()
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        // printf("LED %d: %08X\n", i, led_data[i]); // Debug print each LED's color
        pio_sm_put_blocking(pio, stateMachine, led_data[i]);
    }
}

void LED::set_multiple_leds(...)
{
    va_list args;
    va_start(args, args); // Initialize the va_list

    while (true)
    {
        int led_index = va_arg(args, int);

        // Check if led_index is invalid or the argument list has ended
        if (led_index < 0 || led_index >= NUMBER_OF_LEDS)
        {
            break; // Stop processing if the index is out of range
        }

        uint32_t color = va_arg(args, uint32_t); // Fetch the color
        led_data[led_index] = color;             // Update the LED data
    }

    va_end(args); // Clean up the va_list

    update_led(); // Apply the changes to all LEDs
}

void LED::shift_led_colors_right(int led_index)
{
    if (led_index < 0 || led_index >= NUMBER_OF_LEDS - 1)
    {
        return; // Invalid index or no LED to shift to the right
    }

    led_data[led_index + 1] = led_data[led_index]; // Move the color to the right
    led_data[led_index] = 0; // Turn off the original LED

    update_led(); // Apply the changes to the LEDs
}

void LED::shift_led_colors_left(int led_index)
{
    if (led_index <= 0 || led_index >= NUMBER_OF_LEDS)
    {
        return; // Invalid index or no LED to shift to the left
    }

    led_data[led_index - 1] = led_data[led_index]; // Move the color to the left
    led_data[led_index] = 0; // Turn off the original LED

    update_led(); // Apply the changes to the LEDs
}
