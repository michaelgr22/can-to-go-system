/*========== Includes =======================================================*/

#include "controllers/fsm_controller/fsm_controller.h"

#include "esp_log.h"
#include "driver/twai.h"

#include "controllers/display_controller/display_controller.h"
#include "controllers/uart_controller/uart_controller.h"
#include "controllers/can_controller/can_controller.h"
#include "controllers/led_controller/led_controller.h"
#include "controllers/rest_api_controller/rest_api_controller.h"
#include "controllers/display_controller/utils/baudrate_menu/baudrate_menu.h"

/*========== Macros and Definitions =========================================*/

/*========== Static Constant and Variable Definitions =======================*/

enum FsmState fsm_controller_current_state;

static const char *log_tag = "fsm_controller";

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static void fsm_controller_task_handler(void *args)
{
    fsm_controller_current_state = STARTING;
    ESP_LOGI(log_tag, "switched state to STARTING");

    uart_controller_create_task();
    display_controller_create_task();
    led_controller_create_task();
    rest_api_controller_create_task();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    fsm_controller_current_state = CONFIGURATION;
    ESP_LOGI(log_tag, "switched state to CONFIGURATION");

    twai_timing_config_t *p_baudrate = NULL;
    while (p_baudrate == NULL)
    {
        if (xQueueReceive(baudrate_selection_queue, &p_baudrate, portMAX_DELAY) == pdPASS)
        {
            can_controller_create_task(p_baudrate);
        }
        vTaskDelay(10);
    }

    fsm_controller_current_state = OPERATION;
    ESP_LOGI(log_tag, "switched state to OPERATION");

    vTaskDelete(NULL);
}

/*========== Extern Function Implementations ================================*/

extern void fsm_controller_create_task()
{
    xTaskCreate(fsm_controller_task_handler, "fsm_controller", 4096, NULL, configMAX_PRIORITIES - 5, NULL);
}