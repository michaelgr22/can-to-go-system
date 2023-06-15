#ifndef BAUDRATE_MENU_H
#define BAUDRATE_MENU_H

/*========== Includes =======================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "models/display/baudrate_menu_item.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

extern QueueHandle_t baudrate_selection_queue;

extern struct BaudrateMenuItem baudrate_menu[4];

/*========== Extern Function Prototypes =====================================*/

extern void init_baudrate_menu();
extern int get_selected_item_id_of_baudrate_menu();
extern void send_baudrate();

#endif