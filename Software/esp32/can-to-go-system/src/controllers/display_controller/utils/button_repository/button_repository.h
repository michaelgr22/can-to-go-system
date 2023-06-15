#ifndef BUTTON_REPOSITORY_H
#define BUTTON_REPOSITORY_H

/*========== Includes =======================================================*/

/*========== Macros and Definitions =========================================*/

enum ButtonStates
{
    IDLE,
    PRESSED
};

enum Button
{
    BUTTON_DOWN,
    BUTTON_ENTER,
    BUTTON_UP
};

/*========== Extern Constant and Variable Declarations ======================*/

extern int button_states[3];

/*========== Extern Function Prototypes =====================================*/

extern void init_button_repository();
extern int is_button_pressed();

#endif