#include "drivers/WS2812/WS2812.h"
#include "drivers/LIS3DH/LIS3DH.h"
#include "drivers/S08OB381-026/S08OB381-026.h"
#include "drivers/colors.h"
#include "drivers/board.h"
#include "drivers/logging/logging.h"
#include "WS2812.pio.h"

// Other libraries
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <cstdio>
#include <cstring>
#include <vector>
#include "arm_const_structs.h"
#include "arm_math.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "pico/rand.h"
#include "pico/stdlib.h"

using namespace std;

const uint NUMBER_OF_TASKS{5};

volatile int current_task{0}; // Global variable to track the current task

// Flag to track first entry into each task
bool first_entry = true; // Will be reset when switching cases

// ISR for button press to switch between cases
void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == SWITCH_PIN)
    {
        current_task = (current_task + 1) % NUMBER_OF_TASKS;
        first_entry = true; // Set first_entry to true when switching to a new task
    }
}

int main()
{
    stdio_init_all(); // Initialize all stdio functions (e.g., UART, USB)

    // Initialize peripherals
    WS2812 led(LED_PIN, NUMBER_OF_LEDS);
    LIS3DH accel(ACCEL_I2C_ADDRESS, pio0, 0, 0); // Accelerometer for Lab 8
    S08OB381_026 mic;                            // Microphone for FFT

    led.turn_led_off_all();

    // Initialize the microphone driver and FFT instance
    mic.init();
    arm_rfft_instance_q15 microphone_fft_instance;
    // arm_rfft_init_q15(&microphone_fft_instance, 1024, 0, 1); // 0 for forward FFT, 1 for bit reversal

    // Setup the switch GPIO for input with a pull-up resistor
    gpio_init(SWITCH_PIN);
    gpio_set_dir(SWITCH_PIN, GPIO_OUT);
    gpio_pull_up(SWITCH_PIN);

    // Set up an interrupt on the SWITCH_PIN for a falling edge (button press)
    gpio_set_irq_enabled_with_callback(SWITCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    uint8_t read_buffer[6] = {0}; // Buffer for reading accelerometer data

    // Main loop
    while (true)
    {
        switch (current_task)
        {
        case 0: // Lab 7: LED Light Show
        {
            if (first_entry) // Execute this block only the first time the case is entered
            {
                sleep_ms(1000); // 1-second delay for visual effect
                led.turn_led_off_all();
                cout << "Lab 7: LED Light Show." << endl;
                first_entry = false; // Reset the flag to prevent re-execution
            }

            // Generate a random 32-bit color value and select a random LED
            uint32_t random_color = get_rand_32() & 0xFFFFFF00; // 24-bit RGB color value
            uint random_led = get_rand_32() % NUMBER_OF_LEDS;   // Random LED index

            // Set the selected LED to the random color
            led.set_led(random_led, random_color);
            led.update_led();
            sleep_ms(100); // Wait for 100 milliseconds

            // Turn off the LED after a short delay
            led.set_led(random_led, OFF);
            led.update_led();
            sleep_ms(100);

            // Print the LED index and its current color using cout
            cout << "Colour of LED " << setw(2) << random_led
                      << ": 0x" << hex << setw(8) << random_color << dec << endl;

            break;
        }

        case 1: // Lab 8: Accelerometer Level
        {
            // Run this block only once when first entering the case
            if (first_entry)
            {
                sleep_ms(1000);
                cout << "Lab 8: Accelerometer Task." << endl;

                // Perform the register read/write operations
                uint8_t read_buffer[6] = {0}; // Buffer for reading accelerometer data

                // Simulate reading from WHO_AM_I register
                accel.accel_read_registers(WHO_AM_I, read_buffer, 1);
                cout << "WHO_AM_I Register Value: " << static_cast<int>(read_buffer[0]) << endl;

                // Simulate writing to CTRL_REG1 register
                accel.accel_write_register(CTRL_REG1, 0x57);

                first_entry = false; // Set the flag to false after first initialization
            }

            // Run the main accelerometer task continuously
            accel.accelerometer_task(led); // Execute the Lab 8 task to update LEDs
            break;
        }

        case 2: // Lab 10: Microphone FFT
        {
            if (first_entry)
            {
                led.turn_led_off_all();
                sleep_ms(1000);
                cout << "Lab 10: Microphone FFT Analysis Task." << endl;
                first_entry = false; // Set the flag to false after first initialization
            }
            uint16_t microphone_samples[1024]{};
            mic.read(microphone_samples);
            int16_t time_domain[1024]{};
            mic.apply_dc_offset(microphone_samples, time_domain);
            mic.apply_window_function(time_domain);

            q15_t freq_domain[1026]{};
            mic.apply_fft(time_domain, freq_domain);

            q15_t cplx_mag_domain[1026]{};
            mic.get_complex_magnitude(freq_domain, cplx_mag_domain);

            uint32_t bin[12]{};
            mic.process_results(cplx_mag_domain, bin);

            for (int i{}; i < 13; i++)
            {
                bin[i] <<= 8;
            }
            for (int i{}; i < 12; i++)
            {
                led.set_led(i, bin[i]);
                if (bin[i] != 0)
                {
                    cout << "Colour of LED " << setw(2) << i
                              << ": 0x" << hex << setw(8) << bin[i] << dec << endl;
                }
            }

            led.update_led();
            break;
        }

        case 3: // Lab 11: Bluetooth Communication
        {
            if (first_entry)
            {
                sleep_ms(1000);
                first_entry = false;
            }
            cout << "Lab 11: Placeholder Task." << endl;
            break;
        }

        case 4: // Reset flags
        {
            first_entry = true;
            cout << "Entry flag reset." << endl;
            break;
        }

        default:
        {
            if (first_entry)
            {
                sleep_ms(1000);
                first_entry = false;
            }
            cout << "Invalid task." << endl;
            break;
        }
        }

        // Short delay to prevent flooding output
        sleep_ms(10);
    }
}
