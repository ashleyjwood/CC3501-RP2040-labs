#pragma once

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include "arm_math.h"

class WS2812
{
    uint ledPin;
    PIO pio;
    uint stateMachine;
    uint programOffset;
    uint32_t *led_data;
    uint32_t *last_updated_color;
    uint number_of_leds;

public:
    // Constructor
    WS2812(uint ledPin, uint number_of_leds);

    // Function to turn off all LEDs
    void turn_led_off_all();

    // Function to update the LEDs with the current data
    void update_led();

    uint32_t get_last_updated_color(int led_index);
    uint32_t get_pending_color(int led_index);

    // Function to set a single LED
    void set_led(uint led_index, uint32_t color);

    // Variadic template function to update multiple LEDs
    template <typename... Args>
    void set_multiple(int led_index, uint32_t color, Args... args)
    {
        set_led(led_index, color);
        set_multiple(args...);
    }

    // Base case to terminate the recursion
    void set_multiple(int led_index, uint32_t color)
    {
        set_led(led_index, color); // Base case for recursion
    }

    int shift_led_colors_right(int led_index);
    int shift_led_colors_left(int led_index);

    uint32_t convert_hsv_to_rgb(uint16_t hue, float saturation, float value);

    void simulate_fire();

    void update_leds_based_on_samples(uint32_t *led_data, q15_t *samples);

    void update_leds_based_on_pitch(uint32_t *led_data, q15_t *samples, int sampling_rate);

};
