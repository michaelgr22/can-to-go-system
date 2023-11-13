#ifndef CAN_MESSAGE_H
#define CAN_MESSAGE_H

/*========== Includes =======================================================*/

#include <stdio.h>

#include "driver/twai.h"

/*========== Model Declaration =========================================*/
struct CanMessage
{
    int64_t micros;
    twai_message_t message;
};

#endif