#pragma once
#include <stdint.h>

typedef struct __attribute__((packed)){
    uint8_t  node_id;          // Provisioned Source Identifier mapped to a physical zone
    uint16_t co_ppm;           // Carbon monoxide concentration (BM22S3221-1)
    uint16_t co2_ppm;          // Carbon dioxide concentration (STCC4)
    uint16_t iaq_index;        // Unified VOC severity index (BME688 BSEC algorithm)
    int16_t  temperature;      // Ambient temperature (scaled by 100 to avoid float overhead)
    uint16_t humidity;         // Relative humidity (scaled by 100 to avoid float overhead)
    uint32_t pressure;         // Atmospheric pressure natively output in Pascals
    uint8_t  operating_state;  // Node state enumerator (0: Nominal, 1: Elevated, 2: Alarm)
    uint8_t  validation_flags; // Bitmask for hardware health and active sensor fault flags
} telemetry_data_t;