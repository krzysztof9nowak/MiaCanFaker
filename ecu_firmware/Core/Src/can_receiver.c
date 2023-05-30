#include "main.h"
extern volatile inverter_t inverter;
extern volatile charger_t charger;

CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8];
volatile int counter = 0;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    Error_Handler();
  }

  counter++;


  if(RxHeader.StdId == CAN_VAR_STAT1_EGV){
    VAR_Stat1_EGV_t *stat = RxData;
    if (stat->status_word == INVERTER_STATUS_OK)
    {
        inverter.status = INVERTER_STATUS_RUN;
    }
    else
    {
        //inverter.status = INVERTER_STATUS_ERROR;
    }
    inverter.speed = stat->motor_speed;
    inverter.status_word = stat->status_word;
  }

  if(RxHeader.StdId == CAN_VAR_STAT2_EGV){
    VAR_Stat2_EGV_t *stat = RxData;
    inverter.voltage = stat->capacitor_voltage / 16.0;
    inverter.motor_temp = stat->motor_temperature;
    inverter.controller_temp = stat->controler_temperature;
  }

  if(RxHeader.StdId == CAN_VAR_CURRENT_EGV){
    VAR_Current_EGV_t *stat = RxData;
    inverter.current = stat->current / 16.0;
    inverter.motor_current = stat->motor_current;
  }

  if(RxHeader.StdId == CAN_CHA_STATUS_EGV){
    CAN_CHA_Status_EGV_t *stat = RxData;
    charger.status = stat->status;
    charger.presence = stat->presence;
    charger.error = stat->error;
  }

}