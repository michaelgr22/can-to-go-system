/*========== Includes =======================================================*/

#include "display_controller.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "rom/uart.h"

#include "utils/i2c-lcd1602/i2c-lcd1602.h"
#include "utils/smbus/smbus.h"

#include "models/display/baudrate_menu_item.h"
#include "controllers/display_controller/utils/baudrate_menu/baudrate_menu.h"
#include "controllers/display_controller/utils/button_repository/button_repository.h"

/*========== Macros and Definitions =========================================*/

#define LCD_NUM_ROWS 4
#define LCD_NUM_COLUMNS 40
#define LCD_NUM_VISIBLE_COLUMNS 20

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN 0 // disabled
#define I2C_MASTER_RX_BUF_LEN 0 // disabled
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "display_controller";

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static void i2c_master_init()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

i2c_lcd1602_info_t *init_lcd()
{
    // Set up I2C
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = 0x27;

    // Set up the SMBus
    smbus_info_t *smbus_info = smbus_malloc();
    ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
    ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_PERIOD_MS));

    // Set up the LCD1602 device with backlight off
    i2c_lcd1602_info_t *lcd_info = i2c_lcd1602_malloc();
    ESP_ERROR_CHECK(i2c_lcd1602_init(lcd_info, smbus_info, true,
                                     LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VISIBLE_COLUMNS));

    ESP_ERROR_CHECK(i2c_lcd1602_reset(lcd_info));

    // turn on backlight
    i2c_lcd1602_set_backlight(lcd_info, true);

    i2c_lcd1602_set_cursor(lcd_info, true);

    return lcd_info;
}

static void show_baudrate_menu(i2c_lcd1602_info_t *lcd_info)
{

    i2c_lcd1602_reset(lcd_info);
    char selected_string[52] = "> ";

    for (int i = 0; i < (sizeof(baudrate_menu) / sizeof(baudrate_menu[0])); i++)
    {
        i2c_lcd1602_move_cursor(lcd_info, 0, i);

        if (baudrate_menu[i].is_selected)
        {
            strcat(selected_string, baudrate_menu[i].text);
            i2c_lcd1602_write_string(lcd_info, selected_string);
        }
        else
        {
            i2c_lcd1602_write_string(lcd_info, baudrate_menu[i].text);
        }
    }
}

static void update_baudrate_menu(i2c_lcd1602_info_t *lcd_info)
{
    int button_pressed = is_button_pressed();
    int selected_item_id = get_selected_item_id_of_baudrate_menu();
    if (button_pressed >= 0)
    {
        switch (button_pressed)
        {
        case BUTTON_DOWN:
            if (selected_item_id == 0)
            {
                break;
            }
            else
            {
                baudrate_menu[selected_item_id].is_selected = 0;
                baudrate_menu[selected_item_id - 1].is_selected = 1;
            }
            break;
        case BUTTON_ENTER:
            send_twai_timing_config();
            break;
        case BUTTON_UP:
            if (selected_item_id == 3)
            {
                break;
            }
            else
            {
                baudrate_menu[selected_item_id].is_selected = 0;
                baudrate_menu[selected_item_id + 1].is_selected = 1;
            }
            break;
        default:
            break;
        }
        show_baudrate_menu(lcd_info);
    }
}

static void display_controller_task_handler(void *args)
{
    i2c_lcd1602_info_t *lcd_info = init_lcd();
    init_button_repository();
    init_baudrate_menu();

    show_baudrate_menu(lcd_info);

    while (1)
    {
        update_baudrate_menu(lcd_info);

        vTaskDelay(10);
    }
}

/*========== Extern Function Implementations ================================*/

extern void display_controller_create_task()
{
    xTaskCreate(display_controller_task_handler, "display_controller", 4096, NULL, 1, NULL);
}