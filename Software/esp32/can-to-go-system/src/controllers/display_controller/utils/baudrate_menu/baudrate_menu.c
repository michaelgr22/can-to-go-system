/*========== Includes =======================================================*/

#include "baudrate_menu.h"

#include <stdio.h>
#include <string.h>

#include "driver/twai.h"

/*========== Macros and Definitions =========================================*/

#define QUEUE_LENGTH 5
#define ITEM_SIZE sizeof(twai_timing_config_t *)

/*========== Static Constant and Variable Definitions =======================*/

QueueHandle_t baudrate_selection_queue;

struct BaudrateMenuItem baudrate_menu[4];

static const twai_timing_config_t baudrate_125kbits = TWAI_TIMING_CONFIG_125KBITS();
static const twai_timing_config_t baudrate_250kbits = TWAI_TIMING_CONFIG_250KBITS();
static const twai_timing_config_t baudrate_500kbits = TWAI_TIMING_CONFIG_500KBITS();
static const twai_timing_config_t baudrate_1Mbits = TWAI_TIMING_CONFIG_1MBITS();

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

/*========== Extern Function Implementations ================================*/

extern void init_baudrate_menu()
{
    strcpy(baudrate_menu[0].text, "125 kbit/s");
    baudrate_menu[0].id = 0;
    baudrate_menu[0].is_selected = 1;
    baudrate_menu[0].baudrate = &baudrate_125kbits;

    strcpy(baudrate_menu[1].text, "250 kbit/s");
    baudrate_menu[1].id = 1;
    baudrate_menu[1].is_selected = 0;
    baudrate_menu[1].baudrate = &baudrate_250kbits;

    strcpy(baudrate_menu[2].text, "500 kbit/s");
    baudrate_menu[2].id = 2;
    baudrate_menu[2].is_selected = 0;
    baudrate_menu[2].baudrate = &baudrate_500kbits;

    strcpy(baudrate_menu[3].text, "1 Mbit/s");
    baudrate_menu[3].id = 3;
    baudrate_menu[3].is_selected = 0;
    baudrate_menu[3].baudrate = &baudrate_1Mbits;

    baudrate_selection_queue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
}

extern int get_selected_item_id_of_baudrate_menu()
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

extern void send_baudrate()
{
    int selected_item = get_selected_item_id_of_baudrate_menu();

    xQueueSend(baudrate_selection_queue, &baudrate_menu[selected_item].baudrate, portMAX_DELAY);
}