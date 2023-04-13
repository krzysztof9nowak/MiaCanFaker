#include "dashboard.h"
#include "main.h"

void DashboardTask(void *argument){
    HAL_GPIO_WritePin(OLED_POWER_EN_GPIO_Port, OLED_POWER_EN_Pin, 1);
    while(1){
        
    }
}

