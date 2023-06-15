#ifndef BAUDRATE_MENU_H
#define BAUDRATE_MENU_H

/*========== Includes =======================================================*/

#include <stdio.h>
#include <string.h>

#include "models/display/menu_item.h"

/*========== Macros and Definitions =========================================*/

/*========== Extern Constant and Variable Declarations ======================*/

static struct MenuItem baudrate_menu[4];

/*========== Extern Function Prototypes =====================================*/

void init_baudrate_menu()
{
    strcpy(baudrate_menu[0].text, "125 kbit/s");
    baudrate_menu[0].id = 0;
    baudrate_menu[0].is_selected = 1;

    strcpy(baudrate_menu[1].text, "250 kbit/s");
    baudrate_menu[1].id = 1;
    baudrate_menu[1].is_selected = 0;

    strcpy(baudrate_menu[2].text, "500 kbit/s");
    baudrate_menu[2].id = 2;
    baudrate_menu[1].is_selected = 0;

    strcpy(baudrate_menu[3].text, "1 Mbit/s");
    baudrate_menu[3].id = 3;
    baudrate_menu[1].is_selected = 0;
}

int get_selected_item_id_of_baudrate_menu()
{
    int selected_id = -1;
    for (int i = 0; i < (sizeof(baudrate_menu) / sizeof(baudrate_menu[0])); i++)
    {
        if (baudrate_menu[i].is_selected)
        {
            selected_id = baudrate_menu[i].id;
            return selected_id;
            break;
        }
    }
    return -1;
}

#endif