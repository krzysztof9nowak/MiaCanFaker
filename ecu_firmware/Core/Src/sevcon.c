#include "sevcon.h"


extern CAN_HandleTypeDef hcan1;
extern ADC_HandleTypeDef hadc1;

uint16_t swap_endianness(uint16_t value)
{
    uint16_t ret_val;
    uint16_t lsb, msb;
    lsb = value & 0x00FF;
    msb = value & 0xFF00;
    ret_val = (lsb << 16) + (msb >> 16);

    return ret_val;

}



void can_send_egv_sync_all(CAN_EGV_SYNC_ALL_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_SYNC_ALL_ID;
    carrier.DLC = sizeof (CAN_EGV_SYNC_ALL_t);
    HAL_CAN_AddTxMessage(&hcan1,&carrier,(char*)frame,NULL);

}

void can_send_egv_accel_var(CAN_EGV_Accel_VAR_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};

    carrier.StdId = CAN_EGV_ACCEL_VAR_ID;
    carrier.DLC = sizeof (CAN_EGV_Accel_VAR_t);
    HAL_CAN_AddTxMessage(&hcan1,&carrier,(char *)frame,NULL);
}

void can_send_egv_cmd_var(CAN_EGV_Cmd_VAR_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_CMD_VAR_ID;
    carrier.DLC = sizeof (CAN_EGV_Cmd_VAR_t);
    HAL_CAN_AddTxMessage(&hcan1,&carrier,(char *)frame,NULL);

}

uint16_t get_throttle()
{
    HAL_ADC_Start(&hadc1);
    uint32_t reading = HAL_ADC_GetValue(&hadc1);
    return (uint16_t)(reading>>4);
}

void update_accel_pedal(CAN_EGV_Accel_VAR_t * frame)
{
    int direction_forward = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)== GPIO_PIN_RESET);

    frame->accelerator_set_point = get_throttle();
    if(var_ready && frame->accelerator_set_point > 70) {
        // egv_accel_frame.accelerator_set_point = 0;
        frame->footswitch = 1; //?
        frame->regen_max = 0;
        if(direction_forward)
        {
            frame->reverse = 0;
            frame->forward = 1;
        } else
        {
            frame->reverse=1;
            frame->forward = 0;
        }
    } else
    {
        frame->footswitch = 0; //?
        frame->regen_max = 0;
        frame->forward = 0;
        frame->accelerator_set_point = 0;
    }
}