#include "main.h"
extern volatile inverter_t inverter;

CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8];
volatile int counter = 0;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    Error_Handler();
  }

  counter++;


  if(RxHeader.StdId == 0x181){
    HAL_GPIO_TogglePin(LED_HEADLIGHT_GPIO_Port,LED_HEADLIGHT_Pin);

    VAR_Stat1_EGV_t *stat = RxData;
    if (stat->status_word == INVERTER_STATUS_OK)
    {
        inverter.status = INVERTER_STATUS_RUN;
    }
    inverter.speed = stat->motor_speed;
    inverter.status_word = stat->status_word;
  }

}