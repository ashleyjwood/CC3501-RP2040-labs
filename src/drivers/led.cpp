#include "led.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"

LED::LED(uint ledPin, PIO pioInstance, uint sm, uint offset)
    : ledPin(ledPin), pio(pioInstance), stateMachine(sm), programOffset(offset)
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        led_data[i] = 0; // Initialize all LEDs to off
    }
    ws2812_program_init(pio, stateMachine, programOffset, ledPin, 800000, false);
}

void LED::turn_led_on(uint led_index, uint32_t color)
{
    if (led_index < NUMBER_OF_LEDS)
    {
        led_data[led_index] = color;
    }
}

void LED::turn_led_off(uint led_index)
{
    if (led_index < NUMBER_OF_LEDS)
    {
        led_data[led_index] = 0; // Turn off the specified LED
    }
}

void LED::turn_led_off_all()
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        led_data[i] = 0; // Turn off all LEDs
    }
}

void LED::update_led()
{
    for (int i = 0; i < NUMBER_OF_LEDS; ++i)
    {
        pio_sm_put_blocking(pio, stateMachine, led_data[i]);
    }
}
