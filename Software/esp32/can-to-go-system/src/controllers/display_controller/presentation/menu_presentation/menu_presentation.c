/*========== Includes =======================================================*/

#include "menu_presentation.h"

#include <string.h>

/*========== Macros and Definitions =========================================*/

/*========== Static Constant and Variable Definitions =======================*/

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

/*========== Extern Function Implementations ================================*/

extern void menu_presentation_show(i2c_lcd1602_info_t *lcd_info, struct MenuItem menu[], int menu_size)
{
    i2c_lcd1602_reset(lcd_info);
    char selected_string[52] = "> ";

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
