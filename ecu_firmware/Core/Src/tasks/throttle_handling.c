#include "main.h"
#include "u8g2.h"
#include "cmsis_os.h"
#include "throttle_handling.h"
#include "sevcon.h"

extern ADC_HandleTypeDef hadc1;
extern volatile uint32_t display_value;

extern volatile CAN_EGV_Accel_VAR_t egv_accel_frame;

extern volatile CAN_EGV_SYNC_ALL_t egv_sync_frame;

extern volatile CAN_EGV_Cmd_VAR_t egv_var_frame;

void throttle_task(void *argument)
{
    osDelay(100);

    while (1)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,100);
        uint32_t raw_analog = HAL_ADC_GetValue(&hadc1);
        display_value = raw_analog;

        update_accel_pedal(&egv_accel_frame);
        can_send_egv_sync_all(&egv_sync_frame);
        can_send_egv_accel_var(&egv_accel_frame);
        can_send_egv_cmd_var(&can_send_egv_cmd_var);
        HAL_GPIO_TogglePin(LED_BATTERY_HV_GPIO_Port,LED_BATTERY_HV_Pin);
        osDelay(100);


    }
}