#pragma once

#include <cstdint> // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg> // For variadic functions

#define NUMBER_OF_LEDS 12

class LED {
    uint ledPin;
    PIO pio;
    uint stateMachine;
    uint programOffset;
    uint32_t led_data[NUMBER_OF_LEDS]; // Array to hold data for each LED

public:
    // Constructor
    LED(uint ledPin, PIO pioInstance, uint sm, uint offset);

    // Function to turn on a specific LED with a given color
    void turn_led_on(uint led_index, uint32_t color);

    // Function to turn off a specific LED
    void turn_led_off(uint led_index);

    // Function to turn off all LEDs
    void turn_led_off_all();

    // Function to update the LEDs with the current data
    void update_led();

    // Variadic function to update multiple LEDs at once
    void set_multiple_leds(...);

    void shift_led_colors_right(int led_index);
    void shift_led_colors_left(int led_index);
};
