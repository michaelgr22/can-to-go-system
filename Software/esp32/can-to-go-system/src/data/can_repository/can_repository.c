/*========== Includes =======================================================*/

#include "can_repository.h"

#include <string.h>
#include "freertos/queue.h"
#include "esp_log.h"

/*========== Macros and Definitions =========================================*/

#define RECEIVED_MESSAGES_QUEUE_LENGTH 15
#define RECEIVED_MESSAGES_QUEUE_ITEM_SIZE sizeof(twai_message_t)

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "can_repository";
static QueueHandle_t can_repository_received_messages_queue;

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

/*========== Extern Function Implementations ================================*/

extern void can_repository_init()
{
    if (can_repository_received_messages_queue == NULL)
    {
        can_repository_received_messages_queue = xQueueCreate(RECEIVED_MESSAGES_QUEUE_LENGTH, RECEIVED_MESSAGES_QUEUE_ITEM_SIZE);

        ESP_LOGI(log_tag, "can repository initialized");
    }
}

extern void can_repository_send_message(twai_message_t message)
{
    xQueueSend(can_repository_received_messages_queue, &message, 10);
}

extern int can_repository_received_message_to_display(char *display_message_text)
{
    if (can_repository_received_messages_queue != NULL)
    {
        twai_message_t received_message;
        if (xQueueReceive(can_repository_received_messages_queue, &received_message, 1) == pdPASS)
        {
            const int index_data_start = 9;
            sprintf(display_message_text, "0x%lX", received_message.identifier);
            const int needed_whitespaces = index_data_start - strlen(display_message_text);

            for (int i = 0; i < needed_whitespaces; i++)
            {
                strcat(display_message_text, " ");
            }

            int arrayLength = sizeof(received_message.data) / sizeof(received_message.data[0]);

            for (int i = 0; i < arrayLength; i++)
            {
                char item[10];
                snprintf(item, sizeof(item), "%d", received_message.data[i]);
                strcat(display_message_text, item);
            }

            return received_message.identifier;
        }
    }

    return -1;
}