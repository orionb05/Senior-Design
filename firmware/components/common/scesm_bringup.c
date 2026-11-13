#include "scesm_bringup.h"

#include "esp_chip_info.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"

static const char *TAG = "scesm_bringup";

static const char *reset_reason_name(esp_reset_reason_t reason)
{
    switch (reason) {
    case ESP_RST_POWERON:
        return "power-on";
    case ESP_RST_SW:
        return "software";
    case ESP_RST_PANIC:
        return "panic";
    case ESP_RST_INT_WDT:
        return "interrupt-watchdog";
    case ESP_RST_TASK_WDT:
        return "task-watchdog";
    case ESP_RST_WDT:
        return "watchdog";
    case ESP_RST_DEEPSLEEP:
        return "deep-sleep";
    case ESP_RST_BROWNOUT:
        return "brownout";
    case ESP_RST_SDIO:
        return "sdio";
    default:
        return "other";
    }
}

static void print_startup_info(const char *role)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ESP_LOGI(TAG, "SCESM %s starting", role);
    ESP_LOGI(TAG, "chip model=%d revision=%d cores=%d features=0x%lx",
             chip_info.model,
             chip_info.revision,
             chip_info.cores,
             (unsigned long)chip_info.features);
    ESP_LOGI(TAG, "reset reason=%s", reset_reason_name(esp_reset_reason()));
}

#if CONFIG_SCESM_BRINGUP_ENABLE
static void bringup_task(void *arg)
{
    const char *role = (const char *)arg;

    while (true) {
        ESP_LOGI(TAG,
                 "%s heartbeat: uptime_ms=%llu free_heap=%lu",
                 role,
                 (unsigned long long)(esp_timer_get_time() / 1000ULL),
                 (unsigned long)esp_get_free_heap_size());

        vTaskDelay(pdMS_TO_TICKS(CONFIG_SCESM_BRINGUP_INTERVAL_MS));
    }
}
#endif

void scesm_bringup_start(const char *role)
{
    print_startup_info(role);

#if CONFIG_SCESM_BRINGUP_ENABLE
    xTaskCreate(bringup_task, "bringup_task", 3072, (void *)role, 1, NULL);
#else
    ESP_LOGI(TAG, "serial bring-up verification disabled for %s", role);
#endif
}
