#pragma once

#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg>        // For variadic functions

class LED
{
    uint ledPin;
    PIO pio;
    uint stateMachine;
    uint programOffset;
    uint32_t *led_data;
    uint32_t *last_updated_color;
    uint32_t *pending_color;
    uint32_t *current_color;
    bool *is_led_set;
    int NUMBER_OF_LEDS;

public:
    // Constructor
    LED(uint ledPin, PIO pioInstance, uint sm, uint offset, int NUMBER_OF_LEDS);

    // Function to turn off all LEDs
    void turn_led_off_all();

    // Function to update the LEDs with the current data
    void update_led();

    uint32_t get_last_updated_color(int led_index);
    uint32_t get_pending_color(int led_index);

    // Function to set a single LED
    void set_led(int led_index, uint32_t color);

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

    void print_led_status(int led_index);

    void shift_led_colors_right(int led_index);
    void shift_led_colors_left(int led_index);

    uint32_t convert_hsv_rgb(uint16_t hue, float saturation, float value);
};
