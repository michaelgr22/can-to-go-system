#ifndef CAN_MESSAGES_PRESENTATION_H
#define CAN_MESSAGES_PRESENTATION_H

/*========== Includes =======================================================*/

#include "controllers/display_controller/utils/i2c-lcd1602/i2c-lcd1602.h"
#include "controllers/display_controller/utils/button_repository/button_repository.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

/*========== Extern Function Prototypes =====================================*/

extern void can_messages_presentation_show(i2c_lcd1602_info_t *lcd_info, enum Button button_pressed);

#endif