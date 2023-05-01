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
    uint32_t mailbox;
    HAL_CAN_AddTxMessage(&hcan,&carrier,(uint8_t *)frame, &mailbox);
}



void inverter_task(void *arg)
{
    while (1)
    {
        printf("Var speed: %d\r\nvar status %d\r\n", inverter.speed, inverter.status_word);
        inverter.forward = !HAL_GPIO_ReadPin(IN_DRIVE_DIR_2_GPIO_Port, IN_DRIVE_DIR_2_Pin);
        // inverter.forward = HAL_GPIO_ReadPin(IN_ECO_GPIO_Port, IN_ECO_Pin);

        osDelay(1000);
    }
}
