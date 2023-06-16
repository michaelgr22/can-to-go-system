/*========== Includes =======================================================*/

#include "button_repository.h"

#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gpio.h"

/*========== Macros and Definitions =========================================*/

#define GPIO_BUTTON_DOWN GPIO_NUM_27
#define GPIO_BUTTON_ENTER GPIO_NUM_18
#define GPIO_BUTTON_UP GPIO_NUM_35
#define GPIO_BIT_MASK_BUTTONS ((1ULL << GPIO_BUTTON_DOWN) | (1ULL << GPIO_BUTTON_ENTER) | (1ULL << GPIO_BUTTON_UP))

/*========== Static Constant and Variable Definitions =======================*/

static const int debounce_delay_ms = 125;
static const char *log_tag = "button_repository";

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    static uint32_t last_interrupt_time = 0;
    uint32_t current_interrupt_time = esp_timer_get_time() / 1000; // Convert to milliseconds

    if ((current_interrupt_time - last_interrupt_time) < debounce_delay_ms)
    {
        // Ignore interrupt if it occurred too soon after the previous one
        return;
    }

    int button_id = (int)args;
    button_states[button_id] = PRESSED;
    last_interrupt_time = current_interrupt_time;
}

/*========== Extern Variable Definitions ================================*/

int button_states[3] = {0, 0, 0};

/*========== Extern Function Implementations ================================*/

extern void init_button_repository()
{
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);

    gpio_config_t gpio_buttons_conf;
    gpio_buttons_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_buttons_conf.mode = GPIO_MODE_INPUT;
    gpio_buttons_conf.pin_bit_mask = GPIO_BIT_MASK_BUTTONS;
    gpio_buttons_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_buttons_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_buttons_conf);

    gpio_isr_handler_add(GPIO_BUTTON_DOWN, gpio_interrupt_handler, (void *)BUTTON_DOWN);
    gpio_isr_handler_add(GPIO_BUTTON_ENTER, gpio_interrupt_handler, (void *)BUTTON_ENTER);
    gpio_isr_handler_add(GPIO_BUTTON_UP, gpio_interrupt_handler, (void *)BUTTON_UP);

    ESP_LOGI(log_tag, "button repository initialized");
}

extern int is_button_pressed()
{
    for (int i = 0; i < (sizeof(button_states) / sizeof(button_states[0])); i++)
    {
        if (button_states[i] == 1)
        {
            button_states[i] = 0;
            return i; // Found 1 in the array
        }
    }
    return -1; // Did not find 1 in the array
}
