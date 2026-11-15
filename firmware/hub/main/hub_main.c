#include "scesm_bringup.h"
#include "led.h"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "hub"; // Debug tag

static void led_blink_task(void *pvParameters)
{
    for (;;) {
        esp_err_t err = led_toggle();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to toggle LED: %s", esp_err_to_name(err));
            vTaskDelete(NULL);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    scesm_bringup_start("central hub");
    ESP_ERROR_CHECK(led_init());

    // Create LED blinking task for testing
    BaseType_t result = xTaskCreate(
        led_blink_task,
        "led_blink",
        2048,   // Stack size in bytes with ESP-IDF
        NULL,
        5,      // Task priority
        NULL
    );

    // Task init failure debug output
    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create LED blink task");
    }
}
