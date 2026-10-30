/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// Define constants
#ifndef DEFINES

// General
#define ADC_VREF 3.3f
#define LED_DELAY_MS 250

// CE and ADC Values
#define CE_READ_PIN 28
#define CE_ADC_INPUT 2
#define CE_ADC_VREF_DARK 2.5f

#define DEFINES
#endif

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Init the pin for the Light Sensor
int pico_ce_init(void) {
    adc_init();
    adc_gpio_init(CE_READ_PIN);
    adc_select_input(CE_ADC_INPUT);
    return PICO_OK;
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

int main() {
    stdio_init_all();

    int rc = pico_led_init();
    int ce = pico_ce_init();

    hard_assert(rc == PICO_OK);
    hard_assert(ce == PICO_OK);

    const float conversion_factor = ADC_VREF / (1 << 12);
    const float dark_offset = CE_ADC_VREF_DARK / conversion_factor;

    while (true) {
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        uint16_t result = adc_read();
        if (result > dark_offset) {

            // Placeholder for light sensing functionality
            pico_set_led(true);
            sleep_ms(LED_DELAY_MS);
            pico_set_led(false);
            sleep_ms(LED_DELAY_MS);
        }
    }
}
