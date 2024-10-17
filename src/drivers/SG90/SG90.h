#pragma once

#include <stdio.h>

class SG90
{
private:
    uint8_t servo_pin;
    uint8_t cycle_time;
    uint8_t ideal_speed;
    uint8_t slice_num;
    uint8_t channel_num;

public:
    SG90(uint8_t gpio_pin);
    void drive(int8_t angle); // New method to handle the main loop logic
};
