#include "led.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include <cstdarg>
#include <cstdio>

LED::LED(uint ledPin, PIO pioInstance, uint sm, uint offset, int NUMBER_OF_LEDS)
    : ledPin(ledPin), pio(pioInstance), stateMachine(sm), programOffset(offset), NUMBER_OF_LEDS(NUMBER_OF_LEDS)
{
    led_data = new uint32_t[NUMBER_OF_LEDS];           // Allocate memory for the LED data
    last_updated_color = new uint32_t[NUMBER_OF_LEDS]; // Stores last applied colors
    pending_color = new uint32_t[NUMBER_OF_LEDS];
    is_led_set = new bool[NUMBER_OF_LEDS]; // Allocate memory for the LED state
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        led_data[i] = 0;           // Initialize all LEDs to off
        last_updated_color[i] = 0; // Initially no colors have been applied
        is_led_set[i] = false;     // Initialize all LED states to off
    }
    ws2812_program_init(pio, stateMachine, programOffset, ledPin, 800000, false);
}

void LED::turn_led_off_all()
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        led_data[i] = 0;
    }
    update_led();
}

void LED::update_led()
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        pio_sm_put_blocking(pio, stateMachine, led_data[i]);
        last_updated_color[i] = led_data[i]; // Store the color that was applied
        is_led_set[i] = false;
    }
}

uint32_t LED::get_last_updated_color(int led_index)
{
    return last_updated_color[led_index] >> 8;
}

uint32_t LED::get_pending_color(int led_index)
{
    pending_color[led_index] = led_data[led_index]; // Store the pending color
    return pending_color[led_index] >> 8;
}

void LED::set_led(int led_index, uint32_t color)
{
    // Store the color in the led_data array
    led_data[led_index] = pending_color[led_index] = color;
    is_led_set[led_index] = true; // Mark it as set
}

void LED::print_led_status(int led_index)
{
    if (last_updated_color[led_index] != pending_color[led_index])
    {
        printf("LED %d is set but not updated\n", led_index);
    }
    else if (last_updated_color[led_index] == pending_color[led_index])
    {
        printf("LED %d is updated\n", led_index);
    }
    else if (is_led_set[led_index] == false)
    {
        printf("LED %d is not set\n", led_index);
    }
}

void LED::shift_led_colors_right(int led_index)
{
    if (led_index < 0 || led_index >= NUMBER_OF_LEDS - 1)
    {
        return; // Invalid index or no LED to shift to the right
    }

    led_data[led_index + 1] = led_data[led_index]; // Move the color to the right
    led_data[led_index] = 0;                       // Turn off the original LED

    update_led(); // Apply the changes to the LEDs
}

void LED::shift_led_colors_left(int led_index)
{
    if (led_index <= 0 || led_index >= NUMBER_OF_LEDS)
    {
        return; // Invalid index or no LED to shift to the left
    }

    led_data[led_index - 1] = led_data[led_index]; // Move the color to the left
    led_data[led_index] = 0;                       // Turn off the original LED

    update_led(); // Apply the changes to the LEDs
}
