/*========== Includes =======================================================*/

#include "can_messages_presentation.h"

#include <string.h>
#include "esp_log.h"

#include "data/can_repository/can_repository.h"
#include "models/display/message_item.h"

/*========== Macros and Definitions =========================================*/

#define CURRENT_CAN_MESSAGES_SIZE 12
#define DISPLAY_ROWS 4

/*========== Static Constant and Variable Definitions =======================*/

static struct MessageItem current_can_messages[CURRENT_CAN_MESSAGES_SIZE] = {
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""}};

static const char *log_tag = "can_messages_presentation";

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static int find_available_index_in_current_can_messages(int id)
{
    for (int i = 0; i < CURRENT_CAN_MESSAGES_SIZE; i++)
    {
        if (current_can_messages[i].id == id)
        {
            return i;
        }
    }
    for (int i = 0; i < CURRENT_CAN_MESSAGES_SIZE; i++)
    {
        if (current_can_messages[i].id == -1)
        {
            return i;
        }
    }
    return -1;
}

static void update_current_can_messages()
{
    struct MessageItem received_message_item;
    received_message_item.id = can_repository_received_message_to_display(received_message_item.text);

    if (received_message_item.id != -1)
    {
        int index_in_current_can_messages = find_available_index_in_current_can_messages(received_message_item.id);
        if (index_in_current_can_messages != -1)
        {
            current_can_messages[index_in_current_can_messages].id = received_message_item.id;
            strcpy(current_can_messages[index_in_current_can_messages].text, received_message_item.text);
            return;
        }
    }
}

static int is_can_messages_updated(struct MessageItem old_can_messages[])
{
    for (int i = 0; i < CURRENT_CAN_MESSAGES_SIZE; i++)
    {
        if (strcmp(old_can_messages[i].text, current_can_messages[i].text) != 0)
        {
            return 1;
        }
    }
    return 0;
}

static int get_current_top_id(enum Button button)
{
    static int current_top_id = 0;

    switch (button)
    {
    case BUTTON_DOWN:
        if (current_top_id < (CURRENT_CAN_MESSAGES_SIZE - DISPLAY_ROWS))
        {
            current_top_id++;
        }
        break;
    case BUTTON_UP:
        if (current_top_id > 0)
        {
            current_top_id--;
        }
        break;
    case BUTTON_ENTER:
        for (int i = 0; i < CURRENT_CAN_MESSAGES_SIZE; i++)
        {
            current_can_messages[i].id = -1;
            strcpy(current_can_messages[i].text, "");
        }
        current_top_id = 0;
        break;
    default:
        break;
    }

    return current_top_id;
}

/*========== Extern Function Implementations ================================*/

extern void can_messages_presentation_show(i2c_lcd1602_info_t *lcd_info, enum Button button_pressed)
{
    struct MessageItem old_can_messages[CURRENT_CAN_MESSAGES_SIZE];
    for (int i = 0; i < CURRENT_CAN_MESSAGES_SIZE; i++)
    {
        old_can_messages[i].id = current_can_messages[i].id;
        strcpy(old_can_messages[i].text, current_can_messages[i].text);
    }

    update_current_can_messages();

    if (is_can_messages_updated(old_can_messages) || button_pressed >= 0)
    {
        int current_top_id = get_current_top_id(button_pressed);
        i2c_lcd1602_reset(lcd_info);

        for (int i = 0; i < DISPLAY_ROWS; i++)
        {
            i2c_lcd1602_move_cursor(lcd_info, 0, i);
            if (current_can_messages[current_top_id + i].id != -1)
            {
                i2c_lcd1602_write_string(lcd_info, current_can_messages[current_top_id + i].text);
            }
            else
            {
                i2c_lcd1602_write_string(lcd_info, " ");
            }
        }
    }
}