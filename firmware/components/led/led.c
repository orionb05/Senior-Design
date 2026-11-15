#include "led.h"

#include "driver/gpio.h" // Built-in GPIO driver
#include "sdkconfig.h" // Project configuration header

// TODO: Update to support multiple LEDs
static bool s_is_on; // Current state of single LED (on/off)

// Set up the GPIO pin for the LED and turn it off
esp_err_t led_init(void)
{   
    // Set configuration variable for the GPIO pin's mode, pull-up/down resistors, and interrupt type
    const gpio_config_t config = {
        // NOTE: CONFIG_SCESM_LED_GPIO is defined in the project configuration header (sdkconfig.h) via Kconfig
        .pin_bit_mask = 1ULL << CONFIG_SCESM_LED_GPIO, // Choose which pin to use on board
        .mode = GPIO_MODE_OUTPUT, // Use pin to output voltage (turn LED on/off)
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE, // No interrupts from LED pin
    };

    // Set up GPIO with these settings, catch and return error code if it appears
    esp_err_t err = gpio_config(&config); 
    if (err != ESP_OK) {
        return err;
    }

    // Turn off the LED and set the state variable to false
    s_is_on = false;
    return gpio_set_level(CONFIG_SCESM_LED_GPIO, 0);
}

// Set the GPIO pin to turn the LED on or off, and update the state variable if successful
esp_err_t led_set(bool on)
{
    esp_err_t err = gpio_set_level(CONFIG_SCESM_LED_GPIO, on ? 1 : 0);
    if (err == ESP_OK) {
        s_is_on = on;
    }
    return err;
}

// Toggle the LED state by setting it to the opposite of its current state
esp_err_t led_toggle(void)
{
    return led_set(!s_is_on);
}