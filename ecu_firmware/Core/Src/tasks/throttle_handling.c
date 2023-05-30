#include "main.h"
#include "cmsis_os.h"
#include "throttle_handling.h"
#include "sevcon.h"

extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan;
extern inverter_t inverter;
extern osSemaphoreId_t canSemaphoreHandle;



CAN_EGV_Accel_VAR_t egv_accel_var = {0};
CAN_EGV_Cmd_VAR_t egv_cmd_var = {0};
CAN_EGV_SYNC_ALL_t egv_sync = {
    .status = 0xff
};

int can_error_count = 0;

void can_send_egv_sync_all(CAN_EGV_SYNC_ALL_t *frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_SYNC_ALL_ID;
    carrier.DLC = sizeof(CAN_EGV_SYNC_ALL_t);
    uint32_t mailbox;
    osSemaphoreAcquire(canSemaphoreHandle, osWaitForever);
    if(HAL_CAN_AddTxMessage(&hcan, &carrier, (char *)frame, &mailbox) != HAL_OK){
        can_error_count++;
    } 
    osSemaphoreRelease(canSemaphoreHandle);
}

void can_send_egv_accel_var(CAN_EGV_Accel_VAR_t *frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_ACCEL_VAR_ID;
    carrier.DLC = sizeof(CAN_EGV_Accel_VAR_t);
    uint32_t mailbox;
    osSemaphoreAcquire(canSemaphoreHandle, osWaitForever);

    if(HAL_CAN_AddTxMessage(&hcan, &carrier, (char *)frame, &mailbox) != HAL_OK){
        can_error_count++;
    } 
    osSemaphoreRelease(canSemaphoreHandle);

}

void can_send_egv_cmd_var(CAN_EGV_Cmd_VAR_t *frame)
{
    CAN_TxHeaderTypeDef carrier = {0};
    carrier.StdId = CAN_EGV_CMD_VAR_ID;
    carrier.DLC = sizeof(CAN_EGV_Cmd_VAR_t);
    uint32_t mailbox;
    osSemaphoreAcquire(canSemaphoreHandle, osWaitForever);

    if(HAL_CAN_AddTxMessage(&hcan, &carrier, (char *)frame, &mailbox) != HAL_OK){
        can_error_count++;
    }
    osSemaphoreRelease(canSemaphoreHandle);
}

extern bool run;

void throttle_task(void *argument)
{

    static int count = 0;

    can_send_egv_sync_all(&egv_sync);
    can_send_egv_cmd_var(&egv_cmd_var);

    while (1)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 10);
        uint32_t raw_analog = HAL_ADC_GetValue(&hadc1);
        uint32_t throttle = (raw_analog * 255) / 2800;
        if(throttle < 20)  throttle = 0;
        if(throttle > 255) throttle = 255;
        inverter.throttle = throttle;

        if(count % 1 == 0){
            can_send_egv_sync_all(&egv_sync);
        }

        egv_accel_var.DS1 = 1;
        egv_accel_var.DS2 = 1;
        if(inverter.status == INVERTER_STATUS_RUN){
            egv_accel_var.accelerator_set_point = inverter.throttle;
            egv_accel_var.footswitch = inverter.throttle > 0;
            egv_accel_var.forward = inverter.forward;
            egv_accel_var.reverse = !inverter.forward;
        } else {
            egv_accel_var.accelerator_set_point = 0;
            egv_accel_var.footswitch = 0;
            egv_accel_var.forward = 0;
            egv_accel_var.reverse = 0;
        }
        osDelay(1);
        can_send_egv_accel_var(&egv_accel_var);

        if(inverter.status == INVERTER_STATUS_RUN){
            egv_cmd_var.current_limit = 500; // 2640
            egv_cmd_var.regen_limit = -250;
            egv_cmd_var.max_torque_ratio = 1000;
            egv_cmd_var.motor_command = 6000;

        } else {
            egv_cmd_var.current_limit = 0;
            egv_cmd_var.regen_limit = 0;
            egv_cmd_var.max_torque_ratio = 0;
            egv_cmd_var.motor_command = 0;

        }
        if(inverter.fan_enabled)
        {
            if(inverter.motor_temp < 60)
            {
                inverter.fan_enabled = 0;
            } else
            {
                egv_cmd_var.motor_command = 6000;            }
        } else
        {
            if(inverter.motor_temp > 65)
            {
                inverter.fan_enabled = 1;
                egv_cmd_var.motor_command = 6000;

            } else
            {
                egv_cmd_var.motor_command = 0;
            }

        }

        if(count % 1 == 0){        //zmien na kazdy obrot petli
            // printf("Can error count: %d\r\n", can_error_count);
            //        osDelay(3);
            can_send_egv_cmd_var(&egv_cmd_var);
        }

        osDelay(10);

        count ++;
    }
}
