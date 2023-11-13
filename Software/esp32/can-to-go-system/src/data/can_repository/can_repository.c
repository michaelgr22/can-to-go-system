/*========== Includes =======================================================*/

#include "can_repository.h"

#include <string.h>
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "data/models/display/message_item.h"

/*========== Macros and Definitions =========================================*/

#define DISPLAY_MESSAGES_QUEUE_LENGTH 15
#define DISPLAY_MESSAGES_QUEUE_ITEM_SIZE sizeof(struct MessageItem)
#define REST_API_MESSAGES_QUEUE_LENGTH 15
#define REST_API_MESSAGES_QUEUE_ITEM_SIZE sizeof(struct CanMessage)
#define LED_STATUS_QUEUE_LENGTH 15
#define LED_STATUS_QUEUE_ITEM_SIZE sizeof(struct Led)

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "can_repository";

static QueueHandle_t display_messages_queue;
static QueueHandle_t rest_api_messages_queue;
static QueueHandle_t led_status_queue;

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static struct MessageItem twai_message_to_display_message_item(twai_message_t twai_message)
{
    struct MessageItem message_item;

    message_item.id = twai_message.identifier;

    const int index_data_start = 9;
    sprintf(message_item.text, "0x%lX", twai_message.identifier);
    const int needed_whitespaces = index_data_start - strlen(message_item.text);

    for (int i = 0; i < needed_whitespaces; i++)
    {
        strcat(message_item.text, " ");
    }

    int arrayLength = sizeof(twai_message.data) / sizeof(twai_message.data[0]);

    for (int i = 0; i < arrayLength; i++)
    {
        char item[10];
        snprintf(item, sizeof(item), "%d", twai_message.data[i]);
        strcat(message_item.text, item);
    }

    return message_item;
}

static struct CanMessage twai_message_to_can_message(twai_message_t twai_message)
{
    struct CanMessage can_message;

    int64_t uptime_micros = esp_timer_get_time() / 1000;
    ESP_LOGI(log_tag, "Timer: %lld", uptime_micros);
    can_message.micros = uptime_micros;
    can_message.message = twai_message;

    return can_message;
}

/*========== Extern Function Implementations ================================*/

extern void can_repository_init()
{
    if (display_messages_queue == NULL)
    {
        display_messages_queue = xQueueCreate(DISPLAY_MESSAGES_QUEUE_LENGTH, DISPLAY_MESSAGES_QUEUE_ITEM_SIZE);
    }
    if (rest_api_messages_queue == NULL)
    {
        rest_api_messages_queue = xQueueCreate(REST_API_MESSAGES_QUEUE_LENGTH, REST_API_MESSAGES_QUEUE_ITEM_SIZE);
    }
    if (led_status_queue == NULL)
    {
        led_status_queue = xQueueCreate(LED_STATUS_QUEUE_LENGTH, LED_STATUS_QUEUE_ITEM_SIZE);
    }
    ESP_LOGI(log_tag, "can repository initialized");
}

extern void can_repository_distribute_received_message(twai_message_t received_twai_message)
{
    struct MessageItem received_message_item = twai_message_to_display_message_item(received_twai_message);
    xQueueSend(display_messages_queue, &received_message_item, 1);

    struct CanMessage received_can_message = twai_message_to_can_message(received_twai_message);
    xQueueSend(rest_api_messages_queue, &received_can_message, 1);

    struct Led led_blue_receive = {LED_BLUE_RECEIVE, LED_ON};
    xQueueSend(led_status_queue, &led_blue_receive, 1);
}

extern void can_repository_distribute_send_message()
{
    struct Led led_blue_send = {LED_BLUE_SEND, LED_ON};
    xQueueSend(led_status_queue, &led_blue_send, 1);
}

extern void can_repository_distribute_can_bus_status(int is_can_bus_working)
{
    struct Led led_green = {LED_GREEN, LED_ON};
    struct Led led_red = {LED_RED, LED_ON};
    if (is_can_bus_working)
    {
        ESP_LOGI(log_tag, "can bus works successfully");
        led_red.value = LED_OFF;
    }
    else
    {
        ESP_LOGI(log_tag, "can bus is broken");
        led_green.value = LED_OFF;
    }
    xQueueSend(led_status_queue, &led_green, 1);
    xQueueSend(led_status_queue, &led_red, 1);
}

extern int can_repository_received_message_to_display(char *display_message_text)
{
    struct MessageItem received_message_item;
    if (display_messages_queue != NULL)
    {
        if (xQueueReceive(display_messages_queue, &received_message_item, 1) == pdPASS)
        {
            strcpy(display_message_text, received_message_item.text);
            return received_message_item.id;
        }
    }
    return -1;
}

extern void can_repository_received_message_to_rest_api(struct CanMessage *can_message)
{
    if (rest_api_messages_queue != NULL)
    {
        xQueueReceive(rest_api_messages_queue, can_message, 1);
    }
}

extern struct Led can_repository_to_led()
{
    struct Led led;
    if (led_status_queue != NULL)
    {
        if (xQueueReceive(led_status_queue, &led, 1) == pdPASS)
        {
            return led;
        }
    }
    struct Led no_led = {LED_NONE, 0};
    return no_led;
}