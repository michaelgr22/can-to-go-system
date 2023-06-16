#ifndef BAUDRATE_MENU_ITEM_H
#define BAUDRATE_MENU_ITEM_H

/*========== Includes =======================================================*/

#include <stdio.h>

#include "driver/twai.h"

/*========== Model Declaration =========================================*/
struct MenuItem
{
    u_int8_t id;
    u_int8_t is_selected;
    char text[50];
    void *value;
};

#endif