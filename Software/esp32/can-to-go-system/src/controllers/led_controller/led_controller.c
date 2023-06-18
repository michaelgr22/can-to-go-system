/*========== Includes =======================================================*/

#include "led_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "controllers/fsm_controller/fsm_controller.h"
#include "data/can_repository/can_repository.h"
#include "data/models/leds/led.h"

/*========== Macros and Definitions =========================================*/

#define GPIO_BIT_MASK_LEDS ((1ULL << LED_RED) | (1ULL << LED_GREEN) | (1ULL << LED_BLUE_SEND) | (1ULL << LED_BLUE_RECEIVE))

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "led_controller";

TimerHandle_t receive_message_led_timer;
TimerHandle_t send_message_led_timer;

const char receive_message_led_timer_name[] = "CAN_RECEIVE_MESSAGE_TIMER";
const char send_message_led_timer_name[] = "CAN_SEND_MESSAGE_TIMER";

const int receive_message_led_period_ms = 100;
const int send_message_led_period_ms = 100;

int receive_message_led_interrupt_flag = 0;
int send_message_led_interrupt_flag = 0;

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static void receive_message_led_timer_handler(TimerHandle_t timer)
{
    xTimerStopFromISR(timer, 0);
    receive_message_led_interrupt_flag = 1;
}

static void send_message_led_timer_handler(TimerHandle_t timer)
{
    xTimerStopFromISR(timer, 0);
    send_message_led_interrupt_flag = 1;
}

static void gpios_init()
{
    gpio_config_t gpio_leds_conf;
    gpio_leds_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_leds_conf.mode = GPIO_MODE_OUTPUT;
    gpio_leds_conf.pin_bit_mask = GPIO_BIT_MASK_LEDS;
    gpio_leds_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_leds_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_leds_conf);

    gpio_set_level(LED_RED, LED_OFF);
    gpio_set_level(LED_GREEN, LED_OFF);
    gpio_set_level(LED_BLUE_SEND, LED_OFF);
    gpio_set_level(LED_BLUE_RECEIVE, LED_OFF);

    receive_message_led_timer = xTimerCreate(receive_message_led_timer_name, pdMS_TO_TICKS(receive_message_led_period_ms), pdTRUE, 0, receive_message_led_timer_handler);
    send_message_led_timer = xTimerCreate(send_message_led_timer_name, pdMS_TO_TICKS(send_message_led_period_ms), pdTRUE, 0, send_message_led_timer_handler);

    ESP_LOGI(log_tag, "gpios for leds initialized");
}

static void handle_led_update()
{
    struct Led led_to_update = can_repository_to_led();

    switch (led_to_update.gpio)
    {
    case LED_BLUE_RECEIVE:
        gpio_set_level(LED_BLUE_RECEIVE, LED_ON);
        if (xTimerIsTimerActive(receive_message_led_timer) != pdTRUE)
        {
            xTimerStart(receive_message_led_timer, 0);
        }
        break;
    case LED_BLUE_SEND:
        gpio_set_level(LED_BLUE_SEND, LED_ON);
        if (xTimerIsTimerActive(send_message_led_timer) != pdTRUE)
        {
            xTimerStart(send_message_led_timer, 0);
        }
        break;
    default:
        if (led_to_update.gpio != LED_NONE)
        {
            gpio_set_level(led_to_update.gpio, led_to_update.value);
        }
        break;
    }

    if (receive_message_led_interrupt_flag == 1)
    {
        gpio_set_level(LED_BLUE_RECEIVE, LED_OFF);
        receive_message_led_interrupt_flag = 0;
    }
    if (send_message_led_interrupt_flag == 1)
    {
        gpio_set_level(LED_BLUE_SEND, LED_OFF);
        send_message_led_interrupt_flag = 0;
    }
}

static void led_controller_task_handler()
{
    gpios_init();

    while (1)
    {
        switch (fsm_controller_current_state)
        {
        case STARTING:
            break;
        case CONFIGURATION:
            break;
        case OPERATION:
            handle_led_update();
            break;
        }

        vTaskDelay(1);
    }
}

/*========== Extern Function Implementations ================================*/
extern void led_controller_create_task()
{
    xTaskCreate(led_controller_task_handler, "led_controller", 4096, NULL, 1, NULL);
}