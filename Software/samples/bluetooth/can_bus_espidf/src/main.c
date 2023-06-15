#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "driver/uart.h"
#include "driver/twai.h"

#define CAN_TX_PIN GPIO_NUM_26
#define CAN_RX_PIN GPIO_NUM_25

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
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_num, 256, 0, 0, NULL, 0));
}

extern void setupCan()
{

    twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t timing_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&general_config, &timing_config, &filter_config) == ESP_OK)
    {
        ESP_LOGI(log_tag, "Driver installed");
    }
    else
    {
        ESP_LOGE(log_tag, "Failed to install driver");
        return;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        ESP_LOGI(log_tag, "Driver started");
    }
    else
    {
        ESP_LOGE(log_tag, "Failed to start driver");
        return;
    }
}

static void receiveCanMessagesLookup()
{
    twai_message_t message;

    if (twai_receive(&message, pdMS_TO_TICKS(10)) == ESP_OK)
    {

        int arrayLength = sizeof(message.data) / sizeof(message.data[0]);

        // Create a string to store the result
        char result[15] = "";

        // Iterate over the array elements and append the formatted representation
        for (int i = 0; i < arrayLength; i++)
        {
            char item[10];
            snprintf(item, sizeof(item), "%d ", message.data[i]);
            strcat(result, item);
        }

        ESP_LOGI(log_tag, "Message received: %ld %s", message.identifier, result);
    }
}

static void sendCanMessage()
{
    twai_message_t message;
    message.extd = 1;
    message.data_length_code = 8;
    message.identifier = 0x12345;
    message.data[0] = 0x01;
    message.data[1] = 0x01;
    message.data[2] = 0x00;
    message.data[3] = 0x02;
    message.data[4] = 0x06;
    message.data[5] = 0x01;
    message.data[6] = 0x01;
    message.data[7] = 0x01;

    if (twai_transmit(&message, pdMS_TO_TICKS(10)) == ESP_OK)
    {
        ESP_LOGI(log_tag, "Message queued for transmission");
    }
    else
    {
        ESP_LOGE(log_tag, "Failed to queue message for transmission");
    }
}

void app_main()
{
    installUartDriver();
    setupCan();
    int64_t previousMillis = 0UL;
    int64_t interval = 1000000UL;
    while (1)
    {
        int64_t currentMillis = esp_timer_get_time();
        // print messages to the serial monitor
        if (currentMillis - previousMillis > interval)
        {
            ESP_LOGI(log_tag, "Hello World");
            sendCanMessage();
            previousMillis = currentMillis;
        }

        receiveCanMessagesLookup();

        vTaskDelay(1); // Delay for 1 second
    }
}