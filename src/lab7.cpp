#include "drivers/WS2812/WS2812.h"
#include "drivers/colors.h"
#include "pico/rand.h"
#include "board.h"

#include <map>
#include <string>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

constexpr int ON_TIME = 50;
constexpr int OFF_TIME = 50;
constexpr int TEST_LED_1 = 4;
constexpr int TEST_LED_2 = 5;


int main()
{
    stdio_init_all();
    WS2812 WS2812(LED_PIN, NUMBER_OF_LEDS);

    // 0. Reset LED
    WS2812.set_led(TEST_LED_1, OFF);
    sleep_ms(50);
    WS2812.update_led();
    sleep_ms(50);

    // 1. Set a color but don't update
    WS2812.set_multiple(TEST_LED_1, CYAN, TEST_LED_2, RED);
    sleep_ms(50);

    // 2. Print the current color (should be 0 since no update yet)
    uint32_t current_color = WS2812.get_last_updated_color(TEST_LED_1);
    uint32_t pending_color = WS2812.get_pending_color(TEST_LED_1);
    printf("Current color before update: 0x%06X\n", current_color);
    printf("Pending color before update: 0x%06X\n", pending_color);
    sleep_ms(50);

    // 3. Update the LED
    WS2812.update_led();
    sleep_ms(50);

    // 4. Print the color after the update (should match RED)
    current_color = WS2812.get_last_updated_color(TEST_LED_1);
    printf("Current color after first update: 0x%06X\n", current_color);
    sleep_ms(50);

    // 5. Set a new color but don't update
    WS2812.set_led(TEST_LED_1, YELLOW);
    sleep_ms(50);

    // 6. Print the current color (should still be RED, not updated yet)
    current_color = WS2812.get_last_updated_color(TEST_LED_1);
    printf("Current color before second update: 0x%06X\n", current_color);
    sleep_ms(50);
    pending_color = WS2812.get_pending_color(TEST_LED_1);
    printf("Pending color before second update: 0x%06X\n", pending_color);
    sleep_ms(50);

    // 7. Update the LED with the new color
    WS2812.update_led();
    sleep_ms(50);

    // 8. Print the new color after the second update (should now be GREEN)
    current_color = WS2812.get_last_updated_color(TEST_LED_1);
    printf("Current color after second update: 0x%06X\n", current_color);
    sleep_ms(50);

    // 9. Have some fun with the LEDs
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        for (int j = 0; j < NUMBER_OF_LEDS; j++) // Changed the inner loop variable to 'j'
        {
            uint32_t random_color = get_rand_32();
            random_color = random_color & 0xFFFFFF00; // Mask to ensure 24-bit color
            WS2812.set_led(j, random_color);          // Use 'j' for the inner loop
            WS2812.update_led();
            sleep_ms(100);
            WS2812.set_led(j, OFF); // Use 'j' for the inner loop
            WS2812.update_led();
            sleep_ms(100);
            printf("Color of LED %d: 0x%06X\n", j, random_color); // Use %d for integer and pass 'j'
        }
    }

    return 0;
}