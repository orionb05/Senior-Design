#include "storage.h"
#include "telemetry.h" // Telemetry data structure

#include <unistd.h> // For fsync and fileno
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sdkconfig.h"

#include "esp_log.h"

// Define a tag for this component at the top of your file
static const char *STORAGE_TAG = "SD_STORAGE";

// Configure the SD card and mount the FAT filesystem
esp_err_t sd_init(void)
{
    esp_err_t err;

    // Set the SDMMC host controller with default configuration
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_1;
    host.max_freq_khz = 400; // TEST 3: Drop frequency to 400 kHz to survive breadboard capacitance

    // Initialize the SDMMC driver with the host configuration
    // err = sdmmc_host_init(); 
    // if (err != ESP_OK) {
    //     return err;
    // }

    // Set configuration variable for the SDMMC slot to ESP-IDF's default values
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set the GPIO pins for the SDMMC slot to the wired pinout
    slot_config.clk = GPIO_NUM_6;
    slot_config.cmd = GPIO_NUM_5;
    slot_config.d0  = GPIO_NUM_7;
    slot_config.d1  = GPIO_NUM_16;
    slot_config.d2  = GPIO_NUM_13;
    slot_config.d3  = GPIO_NUM_4;
    // TEST 1: Bypass physical Card Detect logic
    slot_config.cd  = GPIO_NUM_15;
    // slot_config.cd  = SDMMC_SLOT_NO_CD;

    // Set the bus width to match the 4-bit DAT0-DAT3 wiring
    slot_config.width = 4; // TEST 2: Fall back to 1-bit mode to isolate DAT1-DAT3 wiring

    // Initialize the SDMMC slot with the host and slot configuration
    // err = sdmmc_host_init_slot(host.slot, &slot_config);
    // if (err != ESP_OK) {
    //     return err;
    // }

    // Define mount configuration for the FAT filesystem on the microSD card
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false, // Set to true to format an unreadable card
        .max_files = 5,                  // Maximum number of concurrently open files
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card; // SD card info

    const char mount_point[] = "/sdcard"; // Base path for the mounted filesystem

    // Mount the FAT filesystem on the SD card to the VFS
    err = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (err == ESP_OK) {
        ESP_LOGI(STORAGE_TAG, "SD card mounted successfully!");
        sdmmc_card_print_info(stdout, card); // Print SD card info to console
    } else {
        ESP_LOGE(STORAGE_TAG, "Failed to mount filesystem: %s", esp_err_to_name(err));
    }

    return err;
}

// Write telemetry data to the SD card
esp_err_t sd_write_telemetry(FILE *file, const telemetry_data_t *data)
{
    if (file == NULL || data == NULL) return ESP_ERR_INVALID_ARG;

    // Write telemetry data to the CSV file
    fprintf(file, "%d,%d,%d,%d,%d,%d,%lu,%d,%d\n", 
        data->node_id,
        data->co_ppm,
        data->co2_ppm,
        data->iaq_index,
        data->temperature, 
        data->humidity, 
        data->pressure,
        data->operating_state,
        data->validation_flags
    );
    
    if (ferror(file)) {
        ESP_LOGE(STORAGE_TAG, "Error writing to file");
        return ESP_FAIL;
    }

    fflush(file); // Ensure data is written to the SD card
    fsync(fileno(file)); // Force the file descriptor to flush to disk

    return ESP_OK;
}

// Read data from the SD card
esp_err_t sd_read_telemetry(FILE *file, telemetry_data_t *data)
{
    if (file == NULL || data == NULL) return ESP_ERR_INVALID_ARG;

    int result = fscanf(file, "%hhu,%hu,%hu,%hu,%hd,%hu,%lu,%hhu,%hhu\n",
        &data->node_id,
        &data->co_ppm,
        &data->co2_ppm,
        &data->iaq_index,
        &data->temperature, 
        &data->humidity, 
        &data->pressure,
        &data->operating_state,
        &data->validation_flags
    );

    if (result != 9) {
        ESP_LOGE(STORAGE_TAG, "Error reading from file");
        return ESP_FAIL;
    }

    return ESP_OK;
}