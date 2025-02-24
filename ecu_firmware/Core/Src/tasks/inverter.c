#include "main.h"
#include "sevcon.h"
#include "cmsis_os.h"
#include <stdio.h>

extern volatile inverter_t inverter;
extern CAN_HandleTypeDef hcan;
extern bool run;
bool was_pressed = false;
//bool already_toggled = false;
typedef enum GEAR
{
    GEAR_REVERSE,
    GEAR_NEUTRAL,
    GEAR_FORWARD,
}gear_t;
volatile gear_t gear;
void gear_toggle(gear_t * gear)
{
    switch (*gear) {
        case GEAR_REVERSE:
            *gear = GEAR_NEUTRAL;
            break;

        case GEAR_NEUTRAL:
            *gear = GEAR_FORWARD;
            break;

        case GEAR_FORWARD:
            *gear = GEAR_REVERSE;
            break;
    }
}
void inverter_task(void *arg)
{
    while (1)
    {
        bool is_pressed_now = !HAL_GPIO_ReadPin(IN_DRIVE_DIR_2_GPIO_Port, IN_DRIVE_DIR_2_Pin);
        if(!was_pressed)
        {
            if(is_pressed_now)
            {
//                was_pressed = true;
                gear_toggle(&gear);
            }
        }
        was_pressed = is_pressed_now;
        // printf("Var speed: %d\r\nvar status %d\r\n", inverter.speed, inverter.status_word);
//        inverter.forward = !HAL_GPIO_ReadPin(IN_DRIVE_DIR_2_GPIO_Port, IN_DRIVE_DIR_2_Pin);
        if(gear == GEAR_FORWARD)
        {
            inverter.forward = true;

        }
        if(gear == GEAR_REVERSE)
        {
            inverter.forward = false;

        }
        if(gear == GEAR_NEUTRAL)
        {
            inverter.neutral = true;
        } else
        {
            inverter.neutral = false;
        }
        if(run == false)
        {
            gear = GEAR_NEUTRAL;
        }
        osDelay(100);
    }
}
