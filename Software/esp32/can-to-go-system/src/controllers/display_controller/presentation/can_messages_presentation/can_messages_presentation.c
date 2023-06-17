/*========== Includes =======================================================*/

#include "can_messages_presentation.h"

#include <string.h>

#include "data/can_repository/can_repository.h"
#include "models/display/message_item.h"

/*========== Macros and Definitions =========================================*/

#define DISPLAY_ROWS 4

/*========== Static Constant and Variable Definitions =======================*/

static struct MessageItem current_can_messages[DISPLAY_ROWS] = {
    {-1, ""},
    {-1, ""},
    {-1, ""},
    {-1, ""}};

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static int find_available_index_in_current_can_messages(int id)
{
    for (int i = 0; i < DISPLAY_ROWS; i++)
    {
        if (current_can_messages[i].id == id)
        {
            return i;
        }
    }
    for (int i = 0; i < DISPLAY_ROWS; i++)
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

/*========== Extern Function Implementations ================================*/

extern void can_messages_presentation_show(i2c_lcd1602_info_t *lcd_info)
{
    update_current_can_messages();
    i2c_lcd1602_reset(lcd_info);

    for (int i = 0; i < DISPLAY_ROWS; i++)
    {
        if (current_can_messages[i].id != -1)
        {
            i2c_lcd1602_move_cursor(lcd_info, 0, i);
            i2c_lcd1602_write_string(lcd_info, current_can_messages[i].text);
        }
    }
}