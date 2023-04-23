#include "sevcon.h"


extern CAN_HandleTypeDef hcan;
extern ADC_HandleTypeDef hadc1;

extern int var_ready;


uint16_t swap_endianness(uint16_t value) //Zrob implementacje dla inta?
{
    uint16_t ret_val;
    uint8_t lsb, msb;
    lsb = value & 0x00FF;
    msb = value & 0xFF00;
    ret_val = (lsb << 8) + (msb >> 8);

    return ret_val;

}



void can_send_egv_sync_all(CAN_EGV_SYNC_ALL_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_SYNC_ALL_ID;
    carrier.DLC = sizeof (CAN_EGV_SYNC_ALL_t);
    uint32_t mailbox;
    HAL_CAN_AddTxMessage(&hcan,&carrier,(char*)frame, &mailbox);

}

void can_send_egv_accel_var(CAN_EGV_Accel_VAR_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_ACCEL_VAR_ID;
    carrier.DLC = sizeof (CAN_EGV_Accel_VAR_t);
    uint32_t mailbox;

    HAL_CAN_AddTxMessage(&hcan,&carrier,(char *)frame, &mailbox);
}

void can_send_egv_cmd_var(CAN_EGV_Cmd_VAR_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_CMD_VAR_ID;
    carrier.DLC = sizeof (CAN_EGV_Cmd_VAR_t);
    uint32_t mailbox;

    HAL_CAN_AddTxMessage(&hcan,&carrier,(char *)frame, &mailbox);

}

uint16_t get_throttle()
{
    HAL_ADC_Start(&hadc1);
    uint32_t reading = HAL_ADC_GetValue(&hadc1);
    return (uint16_t)(reading>>4);
}

void update_accel_pedal(CAN_EGV_Accel_VAR_t * frame)
{
    int direction_forward = 1;

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