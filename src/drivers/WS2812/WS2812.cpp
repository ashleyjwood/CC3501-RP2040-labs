#include "drivers/WS2812/WS2812.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#define pio pio0
#define stateMachine 0

WS2812::WS2812(uint ledPin, uint number_of_leds)
    : ledPin(ledPin), number_of_leds(number_of_leds)
{
    led_data = new uint32_t[number_of_leds];           // Allocate memory for the LED data
    last_updated_color = new uint32_t[number_of_leds]; // Stores last applied colors
    for (int i = 0; i < number_of_leds; ++i)
    {
        led_data[i] = 0;           // Initialize all LEDs to off
        last_updated_color[i] = 0; // Initially no colors have been applied
    }
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, stateMachine, offset, ledPin, 800000, false);
}

void WS2812::turn_led_off_all()
{
    for (int i = 0; i < number_of_leds; ++i)
    {
        led_data[i] = 0;
    }
    update_led();
}

void WS2812::update_led()
{
    for (int i = 0; i < number_of_leds; ++i)
    {
        pio_sm_put_blocking(pio, stateMachine, led_data[i]);
        last_updated_color[i] = led_data[i]; // Store the color that was applied
    }
}

uint32_t WS2812::get_last_updated_color(int led_index)
{
    return last_updated_color[led_index] >> 8;
}

uint32_t WS2812::get_pending_color(int led_index)
{
    return led_data[led_index] >> 8;
}

void WS2812::set_led(uint led_index, uint32_t color)
{
    // Store the color in the led_data array
    led_data[led_index] = color;
}

int WS2812::shift_led_colors_right(int led_index)
{
    if (led_index < 0 || led_index >= number_of_leds - 1)
    {
        return 1; // Invalid index or no LED to shift to the right
    }
    else if (last_updated_color[led_index] >> 8 != led_data[led_index])
    {
        return 2; // LED has not been set
    }
    else
    {

        led_data[led_index + 1] = led_data[led_index]; // Move the color to the right
        led_data[led_index] = 0;                       // Turn off the original LED

        update_led(); // Apply the changes to the LEDs
        return 0;
    }
}

int WS2812::shift_led_colors_left(int led_index)
{
    if (led_index <= 0 || led_index >= number_of_leds)
    {
        return 1; // Invalid index or no LED to shift to the left
    }
    else if (last_updated_color[led_index] >> 8 != led_data[led_index])
    {
        return 2; // LED has not been set
    }
    else
    {

        led_data[led_index - 1] = led_data[led_index]; // Move the color to the left
        led_data[led_index] = 0;                       // Turn off the original LED

        update_led(); // Apply the changes to the LEDs
        return 0;
    }
}

void WS2812::simulate_fire()
{
    for (;;)
    {
        uint16_t saturation = 1.0; // Full saturation
        uint16_t brightness = 1.0; // Full brightness
        for (int i = 0; i < number_of_leds; i++)
        {
            uint16_t random_hue = rand() % 15;
            uint32_t color = convert_hsv_to_rgb(random_hue, saturation, brightness);
            set_led(i, color);
        }

        // Apply the color changes to the LED strip
        update_led();

        uint16_t delay = rand() % 75;
        sleep_ms(delay);
    }
}

uint32_t WS2812::convert_hsv_to_rgb(uint16_t hue, float saturation, float value)
{
    float c = (value * saturation);
    float hue_sector = hue / 60.0;
    float x = c * (1 - fabs(fmod(hue_sector, 2.0f) - 1));
    float red_float = 0;
    float green_float = 0;
    float blue_float = 0;
    switch ((int)hue_sector)
    {
    case 0:
        red_float = c;
        green_float = x;
        blue_float = 0;
        break;
    case 1:
        red_float = x;
        green_float = c;
        blue_float = 0;
        break;
    case 2:
        red_float = 0;
        green_float = c;
        blue_float = x;
        break;
    case 3:
        red_float = 0;
        green_float = x;
        blue_float = c;
        break;
    case 4:
        red_float = x;
        green_float = 0;
        blue_float = c;
        break;
    case 5:
        red_float = c;
        green_float = 0;
        blue_float = x;
        break;
    default:
        break;
    }
    float m = value - c;
    red_float += m, green_float += m, blue_float += m;
    uint8_t red = red_float * 255;
    uint8_t green = green_float * 255;
    uint8_t blue = blue_float * 255;
    uint32_t rgb_colour = (red << 24) | (green << 16) | (blue << 8);
    return rgb_colour;
}

void WS2812::update_leds_based_on_samples(uint32_t *led_data, q15_t *samples)
{
    // Calculate the total volume from FFT samples
    int32_t total_volume = 0;

    // Sum the magnitudes of all FFT samples to get the overall volume
    for (int i = 0; i < 1024; i++)
    {
        total_volume += abs(samples[i]); // Take the absolute value of each FFT sample and accumulate
    }

    // Scale the total volume to determine how many LEDs to light up
    // The scaling factor should be adjusted to match your system's sensitivity and LED count
    int num_leds_to_light = total_volume / 1000; // Experiment with the divisor (e.g., 50000) to adjust sensitivity

    // Ensure that the number of LEDs to light up is within the range
    if (num_leds_to_light > 12)
    {
        num_leds_to_light = 12; // Cap at the total number of LEDs
    }
    if (num_leds_to_light < 0)
    {
        num_leds_to_light = 0; // Ensure no negative value
    }

    // Set the LEDs to green up to the calculated number of LEDs
    for (int i = 0; i < 12; i++)
    {
        if (i < num_leds_to_light)
        {
            set_led(i, *led_data); // Set LEDs to green
        }
        else
        {
            set_led(i, *led_data); // Turn off LEDs beyond the volume threshold
        }
    }

    // Print debug information to verify LED states
    for (int i = 0; i < 12; i++)
    {
        printf("LED %d data: %08x\n", i, led_data[i]); // Print LED data value in hexadecimal
    }

    // Update the LEDs with the new colors
    update_led();

    // Optional: Add a delay to observe changes more clearly
    sleep_ms(100);
}

void WS2812::update_leds_based_on_pitch(uint32_t *led_data, q15_t *samples, int sampling_rate)
{
    // Step 1: Identify the dominant frequency
    int max_magnitude = 0;
    int max_bin = 0;

    // Find the FFT bin with the highest magnitude
    for (int i = 0; i < 1024 / 2; i++) // Only check up to Nyquist frequency
    {
        int magnitude = abs(samples[i]);
        if (magnitude > max_magnitude)
        {
            max_magnitude = magnitude;
            max_bin = i;
        }
    }

    // Step 2: Convert the bin index to frequency
    float dominant_frequency = (max_bin * sampling_rate) / 1024;

    // Debug: Print the dominant frequency
    printf("Dominant Frequency: %.2f Hz\n", dominant_frequency);

    // Step 3: Map the frequency to a position on the LED strip or a color
    // Example: Use frequency to set LED hue (0-360 degrees)
    float hue = (dominant_frequency / (sampling_rate / 2)) * 360.0f; // Scale to [0, 360]

    // Step 4: Set LEDs based on the pitch
    for (size_t i = 0; i < 12; ++i)
    {
        // Use the same hue for all LEDs to show pitch color
        uint32_t color = convert_hsv_to_rgb(hue / 360.0f, 1.0f, 1.0f); // HSV to RGB conversion
        set_led(i, color);
    }

    // Update the LEDs with the new colors
    update_led();
}
