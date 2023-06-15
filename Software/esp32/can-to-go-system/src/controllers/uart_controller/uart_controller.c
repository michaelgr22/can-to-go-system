/*========== Includes =======================================================*/

#include "uart_controller.h"

#include "driver/uart.h"

/*========== Macros and Definitions =========================================*/

/*========== Static Constant and Variable Definitions =======================*/

static const int uart_num = UART_NUM_0;

static const int uart_baudrate = 115200;
static const uart_word_length_t uart_word_length = UART_DATA_8_BITS;
static const uart_parity_t uart_parity = UART_PARITY_DISABLE;
static const uart_stop_bits_t uart_stop_bits = UART_STOP_BITS_1;
static const uart_hw_flowcontrol_t uart_hw_flowcontrol = UART_HW_FLOWCTRL_DISABLE;
static const uart_sclk_t uart_sclk = UART_SCLK_APB;

static const uart_config_t uart_config = {
    .baud_rate = uart_baudrate,
    .data_bits = uart_word_length,
    .parity = uart_parity,
    .stop_bits = uart_stop_bits,
    .flow_ctrl = uart_hw_flowcontrol,
    .source_clk = uart_sclk,
};

/*========== Static Function Prototypes =====================================*/

static void install_uart_driver();
static void uart_controller_task_handler(void *pvParameters);

/*========== Static Function Implementations ================================*/

static void install_uart_driver()
{
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_num, 256, 0, 0, NULL, 0));
}

static void uart_controller_task_handler(void *pvParameters)
{
    install_uart_driver();
    while (1)
    {
        vTaskDelay(10);
    }
}

/*========== Extern Function Implementations ================================*/

extern void uart_controller_create_task()
{
    xTaskCreate(uart_controller_task_handler, "uart_controller", 4096, NULL, 1, NULL);
}