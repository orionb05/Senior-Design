#pragma once // Single include only guard

#include <stdbool.h>
#include "esp_err.h" // Debug error codes
#include "telemetry.h" // Telemetry data structure

esp_err_t sd_init(void);
esp_err_t sd_write_telemetry(FILE *file, const telemetry_data_t *data);
esp_err_t sd_read_telemetry(FILE *file, telemetry_data_t *data);