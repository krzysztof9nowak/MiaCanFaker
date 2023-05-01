#include "main.h"

#define INDICATOR_DELAY (500)

void blinkenTask(void *arg){
    while(1){
        while(HAL_GPIO_ReadPin(IN_INDICATOR_RIGHT_GPIO_Port, IN_INDICATOR_RIGHT_Pin)){
            HAL_GPIO_WritePin(INDIC_RIGHT_GPIO_Port, INDIC_RIGHT_Pin, 1);
            HAL_GPIO_WritePin(LED_INDICATOR_GPIO_Port, LED_INDICATOR_Pin, 1);
            osDelay(INDICATOR_DELAY);
            HAL_GPIO_WritePin(INDIC_RIGHT_GPIO_Port, INDIC_RIGHT_Pin, 0);
            HAL_GPIO_WritePin(LED_INDICATOR_GPIO_Port, LED_INDICATOR_Pin, 0);
            osDelay(INDICATOR_DELAY);
        }

        while(HAL_GPIO_ReadPin(IN_DRIVE_DIR_1_GPIO_Port, IN_DRIVE_DIR_1_Pin)){
            HAL_GPIO_WritePin(INDIC_LEFT_GPIO_Port, INDIC_LEFT_Pin, 1);
            HAL_GPIO_WritePin(LED_INDICATOR_GPIO_Port, LED_INDICATOR_Pin, 1);
            osDelay(INDICATOR_DELAY);
            HAL_GPIO_WritePin(INDIC_LEFT_GPIO_Port, INDIC_LEFT_Pin, 0);
            HAL_GPIO_WritePin(LED_INDICATOR_GPIO_Port, LED_INDICATOR_Pin, 0);
            osDelay(INDICATOR_DELAY);
        }

        osDelay(100);


    }
}