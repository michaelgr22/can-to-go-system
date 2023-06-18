#ifndef CAN_REPOSITORY_H
#define CAN_REPOSITORY_H

/*========== Includes =======================================================*/

#include "driver/twai.h"

#include "data/models/leds/led.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

/*========== Extern Function Prototypes =====================================*/

extern void can_repository_init();
extern void can_repository_distribute_received_message(twai_message_t received_can_message);
extern int can_repository_received_message_to_display(char *display_message_text);
extern struct Led can_repository_to_led();

#endif