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
    NO_BUTTON_PRESSED = -1,
    BUTTON_DOWN = 0,
    BUTTON_ENTER = 1,
    BUTTON_UP = 2
};

/*========== Extern Constant and Variable Declarations ======================*/

extern int button_states[3];

/*========== Extern Function Prototypes =====================================*/

extern void init_button_repository();
extern int is_button_pressed();

#endif