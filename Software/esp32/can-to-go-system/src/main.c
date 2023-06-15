#include "controllers/display_controller/display_controller.h"
#include "controllers/uart_controller/uart_controller.h"

void app_main()
{
    uart_controller_create_task();
    display_controller_create_task();
}