/*========== Includes =======================================================*/

#include "baudrate_menu.h"

#include <stdio.h>
#include <string.h>
#include "esp_log.h"

#include "driver/twai.h"

/*========== Macros and Definitions =========================================*/

#define QUEUE_LENGTH 5
#define ITEM_SIZE sizeof(void *)

/*========== Static Constant and Variable Definitions =======================*/

QueueHandle_t baudrate_selection_queue;
struct MenuItem baudrate_menu[4];
int baudrate_menu_size = 4;

static const char *log_tag = "baudrate_menu";

static twai_timing_config_t baudrate_125kbits = TWAI_TIMING_CONFIG_125KBITS();
static twai_timing_config_t baudrate_250kbits = TWAI_TIMING_CONFIG_250KBITS();
static twai_timing_config_t baudrate_500kbits = TWAI_TIMING_CONFIG_500KBITS();
static twai_timing_config_t baudrate_1Mbits = TWAI_TIMING_CONFIG_1MBITS();

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

/*========== Extern Function Implementations ================================*/

extern void baudrate_menu_init()
{
    strcpy(baudrate_menu[0].text, "125 kbit/s");
    baudrate_menu[0].id = 0;
    baudrate_menu[0].is_selected = 1;
    baudrate_menu[0].value = &baudrate_125kbits;

    strcpy(baudrate_menu[1].text, "250 kbit/s");
    baudrate_menu[1].id = 1;
    baudrate_menu[1].is_selected = 0;
    baudrate_menu[1].value = &baudrate_250kbits;

    strcpy(baudrate_menu[2].text, "500 kbit/s");
    baudrate_menu[2].id = 2;
    baudrate_menu[2].is_selected = 0;
    baudrate_menu[2].value = &baudrate_500kbits;

    strcpy(baudrate_menu[3].text, "1 Mbit/s");
    baudrate_menu[3].id = 3;
    baudrate_menu[3].is_selected = 0;
    baudrate_menu[3].value = &baudrate_1Mbits;

    baudrate_selection_queue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

    ESP_LOGI(log_tag, "baudrate menu initialized");
}

extern void baudrate_menu_send_baudrate(int selected_item)
{
    xQueueSend(baudrate_selection_queue, &baudrate_menu[selected_item].value, portMAX_DELAY);
}