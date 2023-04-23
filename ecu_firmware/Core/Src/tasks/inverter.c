#include "main.h"
#include "sevcon.h"
#include "cmsis_os.h"

#include <stdio.h>



extern volatile inverter_t inverter;
extern CAN_HandleTypeDef hcan;

#define CAN_BMS_CHA_ID 0x622
void can_bms_cha(CAN_BMS_CHA_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};

    carrier.StdId = CAN_BMS_CHA_ID;
    carrier.DLC = 6;

    HAL_CAN_AddTxMessage(&hcan,&carrier,(uint8_t *)frame,NULL);
}

CAN_BMS_CHA_t bms_cha = {
    .max_voltage = 8000,
    .charging_current = 500,
    .status = 0b1 | 0b10,
};

void inverter_task(void *arg)
{


    HAL_GPIO_WritePin(APC_GPIO_Port, APC_Pin, 1);
    HAL_GPIO_WritePin(INVERTER_GPIO_Port, INVERTER_Pin, 1);

    while (1)
    {
        printf("Var speed: %d\r\nvar status %d\r\n", inverter.speed, inverter.status_word);
        inverter.forward = HAL_GPIO_ReadPin(IN_DRIVE_DIR_1_GPIO_Port, IN_DRIVE_DIR_1_Pin);

        // can_bms_cha(&bms_cha);
        osDelay(1000);
    }
}
