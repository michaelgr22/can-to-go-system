/*========== Includes =======================================================*/

#include "menu_presentation.h"

#include <string.h>

/*========== Macros and Definitions =========================================*/

/*========== Static Constant and Variable Definitions =======================*/

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static int get_selected_item_id(struct MenuItem menu[], int menu_size)
{
    int selected_id = -1;
    for (int i = 0; i < menu_size; i++)
    {
        if (menu[i].is_selected)
        {
            selected_id = menu[i].id;
            return selected_id;
        }
    }
    return -1;
}

static void handle_button_pressed(enum Button button_pressed, struct MenuItem menu[], int menu_size, void (*func_enter)(int))
{
    int selected_item_id = get_selected_item_id(menu, menu_size);
    switch (button_pressed)
    {
    case BUTTON_DOWN:
        if (selected_item_id != 3)
        {
            menu[selected_item_id].is_selected = 0;
            menu[selected_item_id + 1].is_selected = 1;
        }
        break;
    case BUTTON_ENTER:
        func_enter(selected_item_id);
        break;
    case BUTTON_UP:
        if (selected_item_id != 0)
        {
            menu[selected_item_id].is_selected = 0;
            menu[selected_item_id - 1].is_selected = 1;
        }
        break;
    default:
        break;
    }
}

/*========== Extern Function Implementations ================================*/

extern void menu_presentation_show(i2c_lcd1602_info_t *lcd_info, struct MenuItem menu[], int menu_size, void (*func_enter)(int), enum Button button_pressed, int initial_show)
{
    char selected_string[52] = "> ";

    if (button_pressed >= 0 || initial_show)
    {
        handle_button_pressed(button_pressed, menu, menu_size, func_enter);

        i2c_lcd1602_reset(lcd_info);
        for (int i = 0; i < menu_size; i++)
        {
            i2c_lcd1602_move_cursor(lcd_info, 0, i);

            if (menu[i].is_selected)
            {
                strcat(selected_string, menu[i].text);
                i2c_lcd1602_write_string(lcd_info, selected_string);
            }
            else
            {
                i2c_lcd1602_write_string(lcd_info, menu[i].text);
            }
        }
    }
}