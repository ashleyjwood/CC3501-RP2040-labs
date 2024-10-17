// Custom header files
#include "drivers/WS2812/WS2812.h"
#include "drivers/colors.h"
#include "drivers/LIS3DH/LIS3DH.h"
#include "drivers/board.h"

// Standard C++ libraries
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "hardware/i2c.h"
#include "drivers/board.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <string>
#include <cstdarg>

#define PADDING 6           // Data right shift padding
#define LEVEL_VARIANCE 0.05 // Threshold for detection around 1g
#define SATURATION 1.0      // Saturation value for HSV
#define VALUE 0.1           // Value for HSV
uint8_t data[8];

// Constructor to initialize the accelerometer and setup I2C
LIS3DH::LIS3DH(uint8_t accel_address, PIO pioInstance, uint sm, uint offset)
    : accel_address(accel_address), pio(pioInstance), stateMachine(sm), programOffset(offset), task_running(false)
{
    // Initialize the I2C bus at 400kHz if it hasn't been initialized elsewhere
    i2c_init(ACCEL_I2C_INSTANCE, 400 * 1000);

    // Set the GPIO functions for the I2C SDA and SCL lines
    gpio_set_function(ACCEL_SDA, GPIO_FUNC_I2C);
    gpio_set_function(ACCEL_SCL, GPIO_FUNC_I2C);

    // Enable pull-up resistors on I2C lines (optional, depending on your hardware)
    gpio_pull_up(ACCEL_SDA);
    gpio_pull_up(ACCEL_SCL);
}

// Write data to the accelerometer
bool LIS3DH::accel_write_register(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = reg;  // Register to write to
    buffer[1] = data; // Data to write to register
    int bytes_written = i2c_write_blocking(ACCEL_I2C_INSTANCE, ACCEL_I2C_ADDRESS, buffer, 2, false);
    if (bytes_written != 2)
    {
        log(LogLevel::ERROR, "Failed to write to accelerometer register.");
        return false;
    }
    return true;
}

// Read data from the accelerometer
bool LIS3DH::accel_read_registers(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= 0x80; // Set the MSB to enable multiple byte read

    // Tell the device which address we want to read
    if (1 != i2c_write_blocking(ACCEL_I2C_INSTANCE, ACCEL_I2C_ADDRESS, &reg, 1, true))
    {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to select register address.");
        return false;
    }

    // Now read the data
    int bytes_read = i2c_read_blocking(ACCEL_I2C_INSTANCE, ACCEL_I2C_ADDRESS, data, length, false);
    if (bytes_read != length)
    {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to read data.");
        return false;
    }
    return true;
}

// Determine if the magnitude is close to 1g within a threshold
bool LIS3DH::is_within_threshold(double accel_value, double target_value, double threshold)
{
    return fabs(accel_value - target_value) <= threshold;
}

// Set the LEDs on the axis that is passed in, based on the acceleration value
void LIS3DH::set_axis_leds(WS2812 &ws2812, std::string axis, double accel_value, double threshold)
{
    int start_index = 0;
    int end_index = 5;

    if (axis == "y_axis")
    {
        start_index = 6;
        end_index = 11;
    }

    // Map the accel_value to an LED index from start_index to end_index
    if (accel_value < -threshold) // Too far in the negative direction
    {
        int led_index = start_index;
        if (accel_value < -2.0 * threshold)
            led_index = start_index + 1;
        if (accel_value < -4.0 * threshold)
            led_index = start_index + 2;
        if (accel_value < -6.0 * threshold)
            led_index = start_index + 3;
        if (accel_value < -8.0 * threshold)
            led_index = start_index + 4;
        if (accel_value < -10.0 * threshold)
            led_index = start_index + 5;

        // Set the LEDs
        for (int i = start_index; i <= end_index; i++)
        {
            if (i == led_index)
                ws2812.set_led(i, ws2812.convert_hsv_to_rgb(0, SATURATION, VALUE));
            else
                ws2812.set_led(i, OFF);
        }
    }
    else if (accel_value > threshold) // Too far in the positive direction
    {
        int led_index = end_index;
        if (accel_value > 2.0 * threshold)
            led_index = end_index - 1;
        if (accel_value > 4.0 * threshold)
            led_index = end_index - 2;
        if (accel_value > 6.0 * threshold)
            led_index = end_index - 3;
        if (accel_value > 8.0 * threshold)
            led_index = end_index - 4;
        if (accel_value > 10.0 * threshold)
            led_index = end_index - 5;

        // Set the LEDs
        for (int i = start_index; i <= end_index; i++)
        {
            if (i == led_index)
                ws2812.set_led(i, ws2812.convert_hsv_to_rgb(0, SATURATION, VALUE));
            else
                ws2812.set_led(i, OFF);
        }
    }
    else // Within the level threshold (centered)
    {
        // Turn all LEDs green when within threshold
        for (int i = start_index; i <= end_index; i++)
        {
            ws2812.set_led(i, ws2812.convert_hsv_to_rgb(120, SATURATION, VALUE)); // Green for all LEDs
        }
    }
}

void LIS3DH::accelerometer_task(WS2812 &ws2812)
{

    // Reading acceleration data
    uint8_t raw_data[6];

    accel_read_registers(0x28, raw_data, 6);

    double SENSITIVITY = 4.0 / 1000; // Adjust for the sensitivity of the accelerometer (apparently this is 4mg/LSB, but seems to track to 1mg/LSB)

    int16_t accel_x_raw = (int16_t)((raw_data[1] << 8) | raw_data[0]) >> PADDING;
    int16_t accel_y_raw = (int16_t)((raw_data[3] << 8) | raw_data[2]) >> PADDING;
    int16_t accel_z_raw = (int16_t)((raw_data[5] << 8) | raw_data[4]) >> PADDING;

    double accel_x = accel_x_raw * SENSITIVITY;
    double accel_y = accel_y_raw * SENSITIVITY;
    double accel_z = accel_z_raw * SENSITIVITY;

    // Output acceleration data for debugging
    std::cout << "Acceleration X: " << accel_x << ", Y: " << accel_y << ", Z: " << accel_z << std::endl;

    // Control X, Y, and Z axis LED groups
    set_axis_leds(ws2812, "x_axis", accel_x, LEVEL_VARIANCE);
    set_axis_leds(ws2812, "y_axis", accel_y, LEVEL_VARIANCE);

    ws2812.update_led();
    busy_wait_ms(100);
}