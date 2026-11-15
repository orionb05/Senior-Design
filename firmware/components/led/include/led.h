#pragma once // Single include only guard

#include <stdbool.h>
#include "esp_err.h" // Debug error codes

esp_err_t led_init(void);
esp_err_t led_set(bool on);
esp_err_t led_toggle(void);