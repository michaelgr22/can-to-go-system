#ifndef BAUDRATE_MENU_H
#define BAUDRATE_MENU_H

/*========== Includes =======================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "models/display/menu_item.h"
#include "controllers/display_controller/utils/button_repository/button_repository.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

extern QueueHandle_t baudrate_selection_queue;

extern int baudrate_menu_size;
extern struct MenuItem baudrate_menu[4];

/*========== Extern Function Prototypes =====================================*/

extern void baudrate_menu_init();
extern void baudrate_menu_handle_button_pressed(enum Button pressed_button);

#endif