idf_component_register(
    SRCS "scesm_bringup.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_system esp_timer freertos log
)
