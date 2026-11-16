# Storage component build configuration will be completed with the first ESP-IDF app.
idf_component_register(
    SRCS "storage.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_sdmmc sdmmc telemetry fatfs # SD card reading/writing depends on the built-in SDMMC driver
)