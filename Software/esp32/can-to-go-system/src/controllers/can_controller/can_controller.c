/*========== Includes =======================================================*/

#include "can_controller.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/twai.h"

/*========== Macros and Definitions =========================================*/

#define CAN_TX_PIN GPIO_NUM_26
#define CAN_RX_PIN GPIO_NUM_25

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "can_controller";

/*========== Static Function Prototypes =====================================*/

static void init_can(twai_timing_config_t *p_twai_timing_config)
{
    twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t timing_config = *p_twai_timing_config;
    twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&general_config, &timing_config, &filter_config) == ESP_OK)
    {
        ESP_LOGI(log_tag, "Driver installed");
    }
    else
    {
        ESP_LOGE(log_tag, "Failed to install driver");
        return;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        ESP_LOGI(log_tag, "Driver started");
    }
    else
    {
        ESP_LOGE(log_tag, "Failed to start driver");
        return;
    }
}

/*========== Static Function Implementations ================================*/
static void receive_can_messages_lookup()
{
    twai_message_t message;

    if (twai_receive(&message, pdMS_TO_TICKS(10)) == ESP_OK)
    {

        int arrayLength = sizeof(message.data) / sizeof(message.data[0]);

        // Create a string to store the result
        char result[15] = "";

        // Iterate over the array elements and append the formatted representation
        for (int i = 0; i < arrayLength; i++)
        {
            char item[10];
            snprintf(item, sizeof(item), "%d ", message.data[i]);
            strcat(result, item);
        }

        ESP_LOGI(log_tag, "Message received: %ld %s", message.identifier, result);
    }
}

static void can_controller_task_handler(void *args)
{
    twai_timing_config_t *p_baudrate = (twai_timing_config_t *)args;
    ESP_LOGI(log_tag, "can selected item: %p", p_baudrate);

    init_can(p_baudrate);

    while (1)
    {
        receive_can_messages_lookup();
        vTaskDelay(10);
    }
}

/*========== Extern Function Implementations ================================*/

extern void can_controller_create_task(twai_timing_config_t *selected_item_id)
{
    xTaskCreate(can_controller_task_handler, "can_controller", 4096, (void *)selected_item_id, 1, NULL);
}