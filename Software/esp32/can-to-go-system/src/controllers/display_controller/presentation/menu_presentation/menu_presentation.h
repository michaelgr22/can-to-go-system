#ifndef MENU_PRESENTATION_H
#define MENU_PRESENTATION_H

/*========== Includes =======================================================*/

#include "controllers/display_controller/utils/i2c-lcd1602/i2c-lcd1602.h"
#include "controllers/display_controller/utils/button_repository/button_repository.h"
#include "data/models/display/menu_item.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

/*========== Extern Function Prototypes =====================================*/

extern void menu_presentation_show(i2c_lcd1602_info_t *lcd_info, struct MenuItem menu[], int menu_size, void (*func_enter)(int), enum Button button_pressed, int initial_show);

#endif