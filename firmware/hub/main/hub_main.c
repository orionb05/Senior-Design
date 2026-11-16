#include "scesm_bringup.h"
#include "led.h"
#include "storage.h"
#include "telemetry.h"

#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "hub"; // Debug tag

QueueHandle_t telemetry_queue;

// Task to store telemetry data in a ring buffer to then be sent to the microSD card
static void data_log_task(void *pvParameters)
{
    telemetry_data_t telemetry_record;

    // Open the CSV file for appending telemetry data
    FILE *file = fopen("/sdcard/data.csv", "a");
    if (file != NULL) {
        // Move the file pointer to the very end of the file
        fseek(file, 0, SEEK_END);
        
        // Read the current position of the pointer (= total file size in bytes)
        long file_size = ftell(file);
        
        ESP_LOGI(TAG, "The current CSV log size is %ld bytes.\n", file_size);

        if (file_size == 0) {
            // If the file is empty, write the header row
            fprintf(file, "Node ID,CO (ppm),CO2 (ppm),IAQ Index,Temperature (C),Humidity (%%),Pressure (Pa),Operating State,Validation Flags\n");
            fflush(file); // Ensure the header is written to the file
            ESP_LOGI(TAG, "CSV header written to data.csv");
        }
    } else {
        ESP_LOGE(TAG, "Failed to open data.csv");
    }

    while(1) {
        // Wait for telemetry data to be available in the queue
        if (xQueueReceive(telemetry_queue, &telemetry_record, portMAX_DELAY) == pdTRUE) {
            // Debug: Print the received data
            ESP_LOGI(TAG, "Received telemetry data: Node ID: %d, CO: %d ppm, CO2: %d ppm, IAQ: %d, Temp: %.2f C, Humidity: %.2f%%, Pressure: %d Pa, State: %d, Flags: 0x%02X",
                     telemetry_record.node_id,
                     telemetry_record.co_ppm,
                     telemetry_record.co2_ppm,
                     telemetry_record.iaq_index,
                     telemetry_record.temperature / 100.0,
                     telemetry_record.humidity / 100.0,
                     telemetry_record.pressure,
                     telemetry_record.operating_state,
                     telemetry_record.validation_flags);
            // Process the telemetry data (e.g., write to microSD card)
            sd_write_telemetry(file, &telemetry_record);
            
            // Read back the file size to confirm the write[cite: 1, 2]
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            ESP_LOGI(TAG, "Write successful. Current CSV size: %ld bytes", file_size);
        }
    }
}

static void led_blink_task(void *pvParameters)
{
    while(1) {
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

    // Allocate a ring buffer capable of holding 20 telemetry data packets in RAM
    telemetry_queue = xQueueCreate(20, sizeof(telemetry_data_t));

    if (telemetry_queue == NULL) {
        // Handle memory allocation failure
        ESP_LOGE(TAG, "Failed to create telemetry queue");
        return;
    }

    // Initialize the microSD card
    ESP_ERROR_CHECK(sd_init());

    // Create LED blinking task for testing
    BaseType_t led_result = xTaskCreatePinnedToCore(
        led_blink_task,
        "led_blink",
        2048,   // Stack size in bytes
        NULL,
        5,      // Task priority
        NULL,
        1       // Pin to core 1
    );

    // Task init failure debug output
    if (led_result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create LED blink task");
    }

    BaseType_t data_log_result = xTaskCreatePinnedToCore(
        data_log_task,
        "data_log",
        4096,   // Stack size in bytes
        NULL,
        4,      // Task priority
        NULL,
        1       // Pin to core 1
    );

    // Task init failure debug output
    if (data_log_result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create data log task");
    }

    // ** DEBUG TEST **: Send a test telemetry data packet to the queue

    telemetry_data_t test_record = {
        .node_id = 42,
        .co_ppm = 15,
        .co2_ppm = 850,
        .iaq_index = 100,
        .temperature = 2250, // 22.50 C
        .humidity = 4500,    // 45.00 %
        .pressure = 101325,  
        .operating_state = 0,
        .validation_flags = 0x00
    };

    ESP_LOGI(TAG, "Simulating BLE Ingestion: Pushing test record to queue...");
    // Push the struct to the queue. The Log Task will unblock and process it[cite: 1]
    xQueueSend(telemetry_queue, &test_record, 0);
}
