#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/twai.h"

#include "controllers/display_controller/display_controller.h"
#include "controllers/uart_controller/uart_controller.h"
#include "controllers/display_controller/utils/baudrate_menu/baudrate_menu.h"
#include "controllers/can_controller/can_controller.h"

static const char *log_tag = "fsm_controller";

void app_main()
{
    uart_controller_create_task();
    display_controller_create_task();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    while (1)
    {

        twai_timing_config_t *p_baudrate;
        if (xQueueReceive(baudrate_selection_queue, &p_baudrate, portMAX_DELAY) == pdPASS)
        {
            ESP_LOGI(log_tag, "selected item can: %p", p_baudrate);
            can_controller_create_task(p_baudrate);
        }

        vTaskDelay(10);
    }
}