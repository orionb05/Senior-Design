# LED component build configuration will be completed with the first ESP-IDF app.
idf_component_register(
    SRCS "led.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_gpio # LED driving depends on the built-in GPIO driver
)