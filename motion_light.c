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

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#ifndef CE_READ_PIN
#define CE_READ_PIN 28
#endif

#ifndef CE_ADC_INPUT
#define CE_ADC_INPUT 2
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

// Init the read pin
int pico_ce_init(void) {
    // gpio_init(CE_READ_PIN);
    // gpio_set_dir(CE_READ_PIN, GPIO_IN);
    // gpio_disable_pulls(CE_READ_PIN);
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

    printf("Raw value, voltage (V)\n");

    while (true) {
        // if (gpio_get(CE_READ_PIN)) {
        //     pico_set_led(true);
        //     sleep_ms(LED_DELAY_MS);
        //     pico_set_led(false);
        //     sleep_ms(LED_DELAY_MS);
        // }

        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        printf("0x%03x, %f\n", result, result * conversion_factor);

        // Flash LED so we know the loop is running
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
        // sleep_ms(500);
    }
}
