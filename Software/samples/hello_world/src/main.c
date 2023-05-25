#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"

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

static const char *log_tag = "uart_controller";

static void installUartDriver()
{
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, 16, 17, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_num, 256, 0, 0, NULL, 0));
}

void app_main()
{
    installUartDriver();
    int64_t previousMillis = 0UL;
    int64_t interval = 1000000UL;

    while (1)
    {
        int64_t currentMillis = esp_timer_get_time();
        // print messages to the serial monitor
        if (currentMillis - previousMillis > interval)
        {
            ESP_LOGI(log_tag, "Hello World");
        }
        // Add your application code here
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    }
}