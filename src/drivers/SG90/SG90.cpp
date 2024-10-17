#include <string.h> // For strcmp()
#include <cstdint>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "drivers/logging/logging.h"
#include "drivers/SG90/SG90.h"

#define CCW_PULSE_WIDTH 1.550e-3
#define CW_PULSE_WIDTH 1.423e-3
#define RP2040_CLOCK_FREQUENCY 125e6
#define CLOCK_DIV 50 // allows for a pwm range of 2.5MHz to 38Hz
#define WRAP (RP2040_CLOCK_FREQUENCY / CLOCK_DIV) / PWM_FREQUENCY
#define PWM_FREQUENCY 50         // Standard for servos (50 Hz, or 20ms period)
#define PWM_PERIOD 20e-3         // 20ms
#define PULSES_PER_90_DEGREES 26 // May need further calibration on final turret

using namespace std;

SG90::SG90(uint8_t gpio_pin)
{
    servo_pin = gpio_pin;
    gpio_set_function(servo_pin, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(servo_pin);
    channel_num = pwm_gpio_to_channel(servo_pin);
    pwm_set_wrap(slice_num, WRAP);
    pwm_set_enabled(slice_num, true);
    pwm_set_clkdiv(slice_num, CLOCK_DIV);
}

void SG90::drive(int8_t angle)
{
    float servo_drive_speed;
    int8_t absolute_angle = abs(angle);
    if (angle > 0)
    {
        servo_drive_speed = CW_PULSE_WIDTH;
    }
    else
    {
        servo_drive_speed = CCW_PULSE_WIDTH;
    }
    uint32_t on_time = ((float)absolute_angle / 90.0) * (PWM_PERIOD * 10e5) * PULSES_PER_90_DEGREES;
    uint16_t pwm_level = (servo_drive_speed / PWM_PERIOD) * WRAP; // Convert pulse width to PWM level
    pwm_set_chan_level(slice_num, channel_num, pwm_level);
    busy_wait_us(on_time);
    // busy_wait_ms(5000);
    pwm_set_chan_level(slice_num, channel_num, 0);
}