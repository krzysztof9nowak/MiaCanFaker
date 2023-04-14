#include "main.h"
#include "u8g2.h"
#include "cmsis_os.h"
#include "throttle_handling.h"

extern ADC_HandleTypeDef hadc1;
extern volatile uint32_t display_value;

void throttle_task(void *argument)
{
    while (1)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,100);
        uint32_t raw_analog = HAL_ADC_GetValue(&hadc1);
        display_value = raw_analog;
    }
}
