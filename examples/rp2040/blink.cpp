/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#undef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25u

// Initialize the GPIO for the LED
static void pico_led_init(void) {
    #ifdef PICO_DEFAULT_LED_PIN
        // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        // gpio_init(PICO_DEFAULT_LED_PIN);
        // gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    #endif
    test::SIO::GPIO_OE_SET::set_bits(test::SIO::GPIO_OE_SET::GPIO_OE_SET_ {
        test::SIO::GPIO_OE_SET::GPIO_OE_SET_::bit{ PICO_DEFAULT_LED_PIN }
    });
    test::SIO::GPIO_OUT_SET::set_bits(test::SIO::GPIO_OUT_SET::GPIO_OUT_SET_ {
        test::SIO::GPIO_OUT_SET::GPIO_OUT_SET_::bit{ PICO_DEFAULT_LED_PIN }
    }
    );
    test::SIO::GPIO_OUT_SET::set_fields_overwrite(test::SIO::GPIO_OUT_SET::GPIO_OUT_SET_::value{ 0U });
}

// Turn the LED on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#endif
}

int main() {
    pico_led_init();
    while (true) {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
    }
}