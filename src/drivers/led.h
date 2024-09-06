#pragma once

#include <cstdint> // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg> // For variadic functions

// #define NUMBER_OF_LEDS 12

class LED {
    uint ledPin;
    PIO pio;
    uint stateMachine;
    uint programOffset;
    uint32_t* led_data;
    uint32_t* last_updated_color;
    uint32_t* pending_color;
    uint32_t* current_color;
    bool* is_led_set;
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

    // Variadic function to update multiple LEDs at once
    // void set_led(...);
    void set_led(int led_index, uint32_t color);

    void print_led_status(int led_index);

    void shift_led_colors_right(int led_index);
    void shift_led_colors_left(int led_index);

    uint32_t get_led_data(int led_index);

};
