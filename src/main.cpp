#include "drivers/led.h"
#include "drivers/colors.h"
#include "pico/rand.h"

#include <map>
#include <string>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#define PICO_LED_PIN 14
#define NUMBER_OF_LEDS 12
#define ON_TIME 50
#define OFF_TIME 50
#define test_led_1 4

// uint32_t random_color = get_rand_32();

int main()
{
    stdio_init_all();
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    LED led(PICO_LED_PIN, pio, 0, offset, NUMBER_OF_LEDS);

    // 0. Reset LED
    led.set_led(test_led_1, OFF);
    sleep_ms(50);
    led.update_led();
    sleep_ms(50);

    // 1. Set a color but don't update
    led.set_led(test_led_1, CYAN);
    sleep_ms(50);

    // 2. Print the current color (should be 0 since no update yet)
    uint32_t current_color = led.get_last_updated_color(test_led_1);
    uint32_t pending_color = led.get_pending_color(test_led_1);
    printf("Current color before update: 0x%06X\n", current_color);
    printf("Pending color before update: 0x%06X\n", pending_color);
    led.print_led_status(test_led_1);
    sleep_ms(50);

    // 3. Update the LED
    led.update_led();
    sleep_ms(50);

    // 4. Print the color after the update (should match RED)
    current_color = led.get_last_updated_color(test_led_1);
    printf("Current color after first update: 0x%06X\n", current_color);
    sleep_ms(50);

    // 5. Set a new color but don't update
    // uint32_t random_color = get_rand_32();
    led.set_led(test_led_1, YELLOW);
    sleep_ms(50);

    // 6. Print the current color (should still be RED, not updated yet)
    current_color = led.get_last_updated_color(test_led_1);
    printf("Current color before second update: 0x%06X\n", current_color);
    sleep_ms(50);
    pending_color = led.get_pending_color(test_led_1);
    printf("Pending color before second update: 0x%06X\n", pending_color);
    sleep_ms(50);

    // 7. Update the LED with the new color
    led.update_led();
    sleep_ms(50);

    // 8. Print the new color after the second update (should now be GREEN)
    current_color = led.get_last_updated_color(test_led_1);
    printf("Current color after second update: 0x%06X\n", current_color);
    led.print_led_status(test_led_1);
    sleep_ms(50);

    return 0;
}
