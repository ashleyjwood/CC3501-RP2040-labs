#include "drivers/S08OB381-026/S08OB381-026.h"
#include "drivers/WS2812/WS2812.h"
#include "drivers/logging/logging.h"
#include <stdio.h>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "arm_math.h"

void S08OB381_026::init()
{
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // Calculate clkdiv to get a sample rate of 44.1 kHz
    adc_set_clkdiv(ClockDivider);

    // Enable the ADC FIFO and configure settings
    adc_fifo_setup(true, false, false, false, false);
    arm_rfft_init_q15(&microphone_fft_instance, 1024, 0, 1);
}

// Method to read samples from the microphone and store them in the provided buffer
void S08OB381_026::read(uint16_t *adc_sample)
{
    adc_run(true); // Enable free-running sampling mode
    for (int i = 0; i < 1024; i++)
    {
        adc_sample[i] = adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();
}

void S08OB381_026::apply_dc_offset(uint16_t *adc_samples, int16_t *time_domain)
{
    // Calculate the mean
    int32_t dc_bias{};
    for (int i = 0; i < 1024; i++)
    {
        dc_bias += adc_samples[i];
    }
    dc_bias /= 1024;

    // Subtract off-set
    for (int i = 0; i < 1024; i++)
    {
        time_domain[i] = (adc_samples[i] - dc_bias) << 3;
    }
}

void S08OB381_026::apply_window_function(int16_t *time_domain)
{
    for (int i = 0; i < 1024; i++)
    {
        time_domain[i] = (static_cast<int32_t>(time_domain[i]) * static_cast<int32_t>(HanningWindow[i])) >> 15;
    }
}
void S08OB381_026::apply_fft(int16_t *time_domain, int16_t *freq_domain)
{
    arm_rfft_q15(&microphone_fft_instance, time_domain, freq_domain);
    for (int32_t i{}; i < 1026; i++)
    {
        freq_domain[i] = static_cast<q15_t>((static_cast<q31_t>(freq_domain[i]) << 9)); // convert 11.5 format to 1.15 format
    }
}

void S08OB381_026::get_complex_magnitude(int16_t *freq_domain, int16_t *complex_magnitude)
{
    arm_cmplx_mag_squared_q15(freq_domain, complex_magnitude, 1026);
    for (int32_t i{}; i < 1026; i++)
    {
        complex_magnitude[i] = static_cast<int16_t>((complex_magnitude[i]) >> 3); // convert 11.5 format to 1.15 format
    }
}

void S08OB381_026::process_results(int16_t *complex_magnitude, uint32_t *bin)
{
    for (int i{6}; i < 8; i++)
    {
        bin[0] += complex_magnitude[i];
    }
    for (int i{8}; i < 11; i++)
    {
        bin[1] += complex_magnitude[i];
    }
    for (int i{11}; i < 16; i++)
    {
        bin[2] += complex_magnitude[i];
    }
    for (int i{16}; i < 24; i++)
    {
        bin[3] += complex_magnitude[i];
    }
    for (int i{24}; i < 35; i++)
    {
        bin[4] += complex_magnitude[i];
    }
    for (int i{35}; i < 51; i++)
    {
        bin[5] += complex_magnitude[i];
    }
    for (int i{51}; i < 75; i++)
    {
        bin[6] += complex_magnitude[i];
    }
    for (int i{75}; i < 110; i++)
    {
        bin[7] += complex_magnitude[i];
    }
    for (int i{110}; i < 161; i++)
    {
        bin[8] += complex_magnitude[i];
    }
    for (int i{161}; i < 237; i++)
    {
        bin[9] += complex_magnitude[i];
    }
    for (int i{237}; i < 349; i++)
    {
        bin[10] += complex_magnitude[i];
    }
    for (int i{349}; i < 513; i++)
    {
        bin[11] += complex_magnitude[i];
    }

    bin[0] /= 2;
    bin[1] /= 3;
    bin[2] /= 5;
    bin[3] /= 8;
    bin[4] /= 11;
    bin[5] /= 16;
    bin[6] /= 24;
    bin[7] /= 35;
    bin[8] /= 51;
    bin[9] /= 76;
    bin[10] /= 112;
    bin[11] /= 163;
}