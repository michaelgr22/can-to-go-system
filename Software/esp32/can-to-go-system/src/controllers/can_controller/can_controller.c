/*========== Includes =======================================================*/

#include "can_controller.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "esp_log.h"
// #include "esp_timer.h"
#include "driver/twai.h"

#include "controllers/fsm_controller/fsm_controller.h"
#include "data/can_repository/can_repository.h"

/*========== Macros and Definitions =========================================*/

#define CAN_TX_PIN GPIO_NUM_26
#define CAN_RX_PIN GPIO_NUM_25

#define CAN_TX_QUEUE_LENGTH 5

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "can_controller";

TimerHandle_t send_test_message_timer;
const char send_test_message_timer_name[] = "SEND_TEST_MESSAGE_TIMER";
int send_test_message_timer_interrupt_flag = 0;
const int send_test_message_timer_period_ms = 100;

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static void send_test_message_timer_interrupt_handler(TimerHandle_t timer)
{
    xTimerStopFromISR(timer, 0);
    send_test_message_timer_interrupt_flag = 1;
}

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

    send_test_message_timer = xTimerCreate(send_test_message_timer_name, pdMS_TO_TICKS(send_test_message_timer_period_ms), pdTRUE, 0, send_test_message_timer_interrupt_handler);
}

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

        can_repository_distribute_received_message(message);
        ESP_LOGI(log_tag, "Message received: %ld %s", message.identifier, result);
    }
}

static int send_can_test_message()
{
    twai_message_t message;
    message.extd = 0;
    message.data_length_code = 8;
    message.identifier = 0x7FF;
    message.data[0] = 0x01;
    message.data[1] = 0x01;
    message.data[2] = 0x01;
    message.data[3] = 0x01;
    message.data[4] = 0x01;
    message.data[5] = 0x01;
    message.data[6] = 0x01;
    message.data[7] = 0x01;

    if (twai_transmit(&message, pdMS_TO_TICKS(10)) == ESP_OK)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int test_can_bus()
{
    static int test_can_bus_message_counter = 0;
    static int test_can_bus_buffer[CAN_TX_QUEUE_LENGTH] = {0, 0, 0, 0, 0};

    if (test_can_bus_message_counter <= CAN_TX_QUEUE_LENGTH - 1)
    {
        test_can_bus_buffer[test_can_bus_message_counter] = send_can_test_message();
        test_can_bus_message_counter++;

        return -1;
    }
    else
    {
        test_can_bus_message_counter = 0;
        for (int i = 0; i < CAN_TX_QUEUE_LENGTH; i++)
        {
            if (test_can_bus_buffer[i] == 0)
            {
                return 0;
            }
        }
        return 1;
    }
}

static void can_controller_task_handler(void *args)
{
    twai_timing_config_t *p_baudrate = (twai_timing_config_t *)args;

    init_can(p_baudrate);
    can_repository_init();
    xTimerStart(send_test_message_timer, 0);

    int test_can_bus_result;
    while (1)
    {
        switch (fsm_controller_current_state)
        {
        case STARTING:
            break;
        case CONFIGURATION:
            break;
        case OPERATION:
            if (send_test_message_timer_interrupt_flag)
            {
                test_can_bus_result = test_can_bus();
                if (test_can_bus_result == 1)
                {
                    ESP_LOGI(log_tag, "can bus works successfully");
                }
                else if (test_can_bus_result == 0)
                {
                    ESP_LOGI(log_tag, "can bus is broken");
                }

                send_test_message_timer_interrupt_flag = 0;
                xTimerStart(send_test_message_timer, 0);
            }
            receive_can_messages_lookup();
            break;
        }
        vTaskDelay(10);
    }
}

/*========== Extern Function Implementations ================================*/

extern void can_controller_create_task(twai_timing_config_t *selected_item_id)
{
    xTaskCreate(can_controller_task_handler, "can_controller", 4096, (void *)selected_item_id, 1, NULL);
}