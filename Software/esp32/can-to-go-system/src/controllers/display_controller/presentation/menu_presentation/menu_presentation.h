#ifndef MENU_PRESENTATION_H
#define MENU_PRESENTATION_H

/*========== Includes =======================================================*/

#include "controllers/display_controller/utils/i2c-lcd1602/i2c-lcd1602.h"
#include "models/display/menu_item.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

/*========== Extern Function Prototypes =====================================*/

extern void menu_presentation_show(i2c_lcd1602_info_t *lcd_info, struct MenuItem menu[], int menu_size);

#endif