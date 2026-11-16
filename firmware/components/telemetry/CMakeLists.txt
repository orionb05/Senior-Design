# Telemetry component for keeping track of sensor readings and other data to be stored in the SD card.
idf_component_register(
    SRCS "telemetry.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos # The telemetry component depends on FreeRTOS for use in queues and other RTOS features.
)