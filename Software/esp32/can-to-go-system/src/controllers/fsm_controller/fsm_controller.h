#ifndef FSM_CONTROLLER_H
#define FSM_CONTROLLER_H

/*========== Includes =======================================================*/

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/*========== Macros and Definitions =========================================*/

enum FsmState
{
    STARTING,
    CONFIGURATION,
    OPERATION,
};

extern enum FsmState fsm_controller_current_state;

/*========== Extern Constant and Variable Declarations ======================*/

/*========== Extern Function Prototypes =====================================*/

extern void fsm_controller_create_task();

#endif