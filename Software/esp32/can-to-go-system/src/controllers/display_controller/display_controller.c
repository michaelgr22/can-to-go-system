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

#include "controllers/display_controller/utils/baudrate_menu/baudrate_menu.h"
#include "controllers/display_controller/utils/button_repository/button_repository.h"
#include "controllers/display_controller/presentation/menu_presentation/menu_presentation.h"
#include "controllers/display_controller/presentation/can_messages_presentation/can_messages_presentation.h"
#include "controllers/fsm_controller/fsm_controller.h"

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

i2c_lcd1602_info_t *lcd_1602_init()
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

    ESP_LOGI(log_tag, "lcd display initialized");

    return lcd_info;
}

static void handle_button_pressed(i2c_lcd1602_info_t *lcd_info)
{
    int button_pressed = is_button_pressed();
    if (button_pressed >= 0)
    {
        baudrate_menu_handle_button_pressed(button_pressed);
        menu_presentation_show(lcd_info, baudrate_menu, baudrate_menu_size);
    }
}

static void display_controller_task_handler(void *args)
{
    i2c_lcd1602_info_t *lcd_info = lcd_1602_init();
    init_button_repository();
    baudrate_menu_init();

    menu_presentation_show(lcd_info, baudrate_menu, baudrate_menu_size);

    while (1)
    {
        switch (fsm_controller_current_state)
        {
        case STARTING:
            break;
        case CONFIGURATION:
            handle_button_pressed(lcd_info);
            break;
        case OPERATION:
            can_messages_presentation_show(lcd_info);
            break;
        }

        vTaskDelay(10);
    }
}

/*========== Extern Function Implementations ================================*/

extern void display_controller_create_task()
{
    xTaskCreate(display_controller_task_handler, "display_controller", 4096, NULL, 1, NULL);
}