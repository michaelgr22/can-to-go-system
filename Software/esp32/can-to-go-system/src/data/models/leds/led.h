#ifndef LED_H
#define LED_H

/*========== Includes =======================================================*/

#include <stdio.h>
#include "driver/gpio.h"

/*========== Model Declaration =========================================*/
enum Leds
{
    LED_NONE = -1,
    LED_RED = GPIO_NUM_33,
    LED_GREEN = GPIO_NUM_4,
    LED_BLUE_SEND = GPIO_NUM_19,
    LED_BLUE_RECEIVE = GPIO_NUM_13,
};

enum LedValue
{
    LED_OFF = 1,
    LED_ON = 0,
};

struct Led
{
    enum Leds gpio;
    enum LedValue value;
};

#endif