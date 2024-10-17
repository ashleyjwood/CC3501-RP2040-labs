#pragma once

// Custom header files
#include "drivers/WS2812/WS2812.h"
#include "drivers/colors.h"

// Standard C++ libraries
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "hardware/i2c.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <string>
#include <cstdarg>
#include <stdint.h>

#define CTRL_REG0 0x1E // Device configuration register
#define CTRL_REG1 0x20 // 400Hz - Data rate configuration
#define CTRL_REG2 0x21 // High-pass filter configuration
#define CTRL_REG3 0x22 // Interrupt configuration
#define CTRL_REG4 0x23 // Full-scale selection
#define CTRL_REG5 0x24 // Turn on temperature sensor
#define CTRL_REG6 0x25 // Interrupt threshold
#define REFERENCE 0x26 // Reference value for interrupt generation
#define WHO_AM_I 0x0f  // Device identification register

class LIS3DH
{
    uint8_t accel_address;
    PIO pio;
    uint stateMachine;
    uint programOffset;

public:
    LIS3DH(uint8_t accel_address, PIO pio, uint stateMachine, uint programOffset);

    bool accel_write_register(uint8_t reg, uint8_t data);

    bool accel_read_registers(uint8_t reg, uint8_t *data, uint8_t length);

    bool is_within_threshold(double accel_value, double target_value, double threshold);

    void set_axis_leds(WS2812 &ws2812, std::string axis, double accel_value, double threshold);

    bool task_running = false;
    void check_switch_state();

    void accelerometer_task(WS2812 &ws2812);
};