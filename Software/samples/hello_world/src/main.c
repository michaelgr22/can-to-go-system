#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define DEBOUNCE_DELAY_MS 125

#define LED_RED GPIO_NUM_33
#define LED_GREEN GPIO_NUM_4
#define LED_BLUE_SEND GPIO_NUM_19
#define LED_BLUE_RECEIVE GPIO_NUM_13
#define GPIO_BIT_MASK_LEDS ((1ULL << LED_RED) | (1ULL << LED_GREEN) | (1ULL << LED_BLUE_SEND) | (1ULL << LED_BLUE_RECEIVE))

#define BUTTON_DOWN GPIO_NUM_27
#define BUTTON_ENTER GPIO_NUM_18
#define BUTTON_UP GPIO_NUM_35
#define GPIO_BIT_MASK_BUTTONS ((1ULL << BUTTON_DOWN) | (1ULL << BUTTON_ENTER) | (1ULL << BUTTON_UP))

static int button_enter_pressed = 0;

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

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    static uint32_t last_interrupt_time = 0;
    uint32_t current_interrupt_time = esp_timer_get_time() / 1000; // Convert to milliseconds

    if ((current_interrupt_time - last_interrupt_time) < DEBOUNCE_DELAY_MS)
    {
        // Ignore interrupt if it occurred too soon after the previous one
        return;
    }

    button_enter_pressed = 1;
    last_interrupt_time = current_interrupt_time;
}

static void configGpiosForLeds()
{
    gpio_config_t gpio_leds_conf;
    gpio_leds_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_leds_conf.mode = GPIO_MODE_OUTPUT;
    gpio_leds_conf.pin_bit_mask = GPIO_BIT_MASK_LEDS;
    gpio_leds_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_leds_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_leds_conf);
}

static void configGpiosForButtons()
{
    gpio_install_isr_service(0);

    gpio_config_t gpio_buttons_conf;
    gpio_buttons_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_buttons_conf.mode = GPIO_MODE_INPUT;
    gpio_buttons_conf.pin_bit_mask = GPIO_BIT_MASK_BUTTONS;
    gpio_buttons_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_buttons_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_buttons_conf);

    gpio_isr_handler_add(GPIO_NUM_18, gpio_interrupt_handler, (void *)GPIO_NUM_18);
}

void app_main()
{
    installUartDriver();
    configGpiosForLeds();
    configGpiosForButtons();
    int64_t previousMillis = 0UL;
    int64_t interval = 1000000UL;
    while (1)
    {
        int64_t currentMillis = esp_timer_get_time();
        // print messages to the serial monitor
        if (currentMillis - previousMillis > interval)
        {
            ESP_LOGI(log_tag, "Hello World");
            previousMillis = currentMillis;
        }

        if (button_enter_pressed)
        {
            ESP_LOGI(log_tag, "Button Enter Pressed");
            button_enter_pressed = 0;
        }

        if (gpio_get_level(BUTTON_DOWN) == 0)
        {                               // If button is pressed
            gpio_set_level(LED_RED, 0); // Turn the LED on
        }
        else
        {
            gpio_set_level(LED_RED, 1);
        }

        if (gpio_get_level(BUTTON_ENTER) == 0)
        {                                     // If button is pressed
            gpio_set_level(LED_BLUE_SEND, 0); // Turn the LED on
            gpio_set_level(LED_GREEN, 0);     // Turn the LED on
        }
        else
        {
            gpio_set_level(LED_BLUE_SEND, 1); // Turn the LED on
            gpio_set_level(LED_GREEN, 1);
        }

        if (gpio_get_level(BUTTON_UP) == 0)
        {                                        // If button is pressed
            gpio_set_level(LED_BLUE_RECEIVE, 0); // Turn the LED on
        }
        else
        {
            gpio_set_level(LED_BLUE_RECEIVE, 1);
        }

        vTaskDelay(1); // Delay for 1 second
    }
}