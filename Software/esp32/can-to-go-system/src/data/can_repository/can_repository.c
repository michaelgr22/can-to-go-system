/*========== Includes =======================================================*/

#include "can_repository.h"

#include <string.h>
#include "freertos/queue.h"
#include "esp_log.h"

#include "data/models/display/message_item.h"

/*========== Macros and Definitions =========================================*/

#define DISPLAY_MESSAGES_QUEUE_LENGTH 15
#define DISPLAY_MESSAGES_QUEUE_ITEM_SIZE sizeof(struct MessageItem)
#define LED_STATUS_QUEUE_LENGTH 15
#define LED_STATUS_QUEUE_ITEM_SIZE sizeof(struct Led)

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "can_repository";

static QueueHandle_t display_messages_queue;
static QueueHandle_t led_status_queue;

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static struct MessageItem can_message_to_display_message_item(twai_message_t can_message)
{
    struct MessageItem message_item;

    message_item.id = can_message.identifier;

    const int index_data_start = 9;
    sprintf(message_item.text, "0x%lX", can_message.identifier);
    const int needed_whitespaces = index_data_start - strlen(message_item.text);

    for (int i = 0; i < needed_whitespaces; i++)
    {
        strcat(message_item.text, " ");
    }

    int arrayLength = sizeof(can_message.data) / sizeof(can_message.data[0]);

    for (int i = 0; i < arrayLength; i++)
    {
        char item[10];
        snprintf(item, sizeof(item), "%d", can_message.data[i]);
        strcat(message_item.text, item);
    }

    return message_item;
}

/*========== Extern Function Implementations ================================*/

extern void can_repository_init()
{
    if (display_messages_queue == NULL)
    {
        display_messages_queue = xQueueCreate(DISPLAY_MESSAGES_QUEUE_LENGTH, DISPLAY_MESSAGES_QUEUE_ITEM_SIZE);
    }
    if (led_status_queue == NULL)
    {
        led_status_queue = xQueueCreate(LED_STATUS_QUEUE_LENGTH, LED_STATUS_QUEUE_ITEM_SIZE);
    }
    ESP_LOGI(log_tag, "can repository initialized");
}

extern void can_repository_distribute_received_message(twai_message_t received_can_message)
{
    struct MessageItem received_message_item = can_message_to_display_message_item(received_can_message);
    xQueueSend(display_messages_queue, &received_message_item, 1);

    struct Led led_blue_send = {LED_BLUE_RECEIVE, LED_ON};
    xQueueSend(led_status_queue, &led_blue_send, 1);
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