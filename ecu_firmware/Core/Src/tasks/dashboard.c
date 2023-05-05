#include "dashboard.h"
#include "main.h"
#include "u8g2.h"
#include "cmsis_os.h"


u8g2_t u8g2;
extern SPI_HandleTypeDef hspi1;
extern volatile inverter_t inverter;

int16_t abs(int16_t x){
	if(x < 0) return -x;
	return x;
}

uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	/* STM32 supports HW SPI, Remove unused cases like U8X8_MSG_DELAY_XXX & U8X8_MSG_GPIO_XXX */
	switch(msg)
	{
	case U8X8_MSG_GPIO_AND_DELAY_INIT:
		/* Insert codes for initialization */
		break;
	case U8X8_MSG_DELAY_MILLI:
		/* ms Delay */
		osDelay(arg_int);
		break;
	case U8X8_MSG_GPIO_CS:
		/* Insert codes for SS pin control */
		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, arg_int);
		break;
	case U8X8_MSG_GPIO_DC:
		/* Insert codes for DC pin control */
		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
		break;
	case U8X8_MSG_GPIO_RESET:
		/* Insert codes for RST pin control */
		HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, arg_int);
		break;
	}
	return 1;
}

#define TX_TIMEOUT		100
uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	switch(msg) {
	case U8X8_MSG_BYTE_SEND:
		/* Insert codes to transmit data */
		if(HAL_SPI_Transmit(&hspi1, arg_ptr, arg_int, TX_TIMEOUT) != HAL_OK) return 0;
		break;
	case U8X8_MSG_BYTE_INIT:
		/* Insert codes to begin SPI transmission */
		break;
	case U8X8_MSG_BYTE_SET_DC:
		/* Control DC pin, U8X8_MSG_GPIO_DC will be called */
		u8x8_gpio_SetDC(u8x8, arg_int);
		break;
	case U8X8_MSG_BYTE_START_TRANSFER:
		/* Select slave, U8X8_MSG_GPIO_CS will be called */
		u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
		osDelay(1);
		break;
	case U8X8_MSG_BYTE_END_TRANSFER:
		osDelay(1);
		/* Insert codes to end SPI transmission */
		u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
		break;
	default:
		return 0;
	}
	return 1;
}


void DashboardTask(void *argument){
    HAL_GPIO_WritePin(OLED_POWER_EN_GPIO_Port, OLED_POWER_EN_Pin, SET);

    u8g2_Setup_ssd1322_nhd_256x64_f(&u8g2, U8G2_R0, u8x8_byte_stm32_hw_spi, u8x8_stm32_gpio_and_delay);

    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    // u8g2_DrawLine(&u8g2, 50,50, 100, 50);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(&u8g2, 70,15, "MIA Electric");
    u8g2_SetFont(&u8g2, u8g2_font_6x13_mr);
    u8g2_DrawStr(&u8g2, 40, 40, "Wilczyckie zaklady przemyslowe");
    u8g2_SendBuffer(&u8g2);

    osDelay(1000);

    char buf[64];

    while(1){
        u8g2_ClearBuffer(&u8g2);
        u8g2_SetFont(&u8g2, u8g2_font_7Segments_26x42_mn);
		const float kmh_per_rpm = 24.0 / 2000.0;
		float kmh = abs((int16_t)inverter.speed) * kmh_per_rpm;
        snprintf(buf, sizeof(buf), "%hd",  (int16_t)kmh);
        u8g2_DrawStr(&u8g2, 100, 50, buf);



		snprintf(buf, sizeof(buf), "%d.%dV", (int)(inverter.voltage), (int)(inverter.voltage * 10) % 10);
		u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
		u8g2_DrawStr(&u8g2, 0, 50, buf);


		snprintf(buf, sizeof(buf), "%dA", (int)(inverter.current));
		u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
		u8g2_DrawStr(&u8g2, 0, 40, buf);

		snprintf(buf, sizeof(buf), "M %dA", (int)(inverter.motor_current));
		u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
		u8g2_DrawStr(&u8g2, 0, 30, buf);

		uint32_t odometer = 40163400;

		snprintf(buf, sizeof(buf), "%d.%dkm", odometer / 1000, (odometer / 100)%10);
		u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
		u8g2_DrawStr(&u8g2, 0, 10, buf);

		snprintf(buf, sizeof(buf), "ctr %dC", inverter.controller_temp);
		u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
		u8g2_DrawStr(&u8g2, 200, 30, buf);

		snprintf(buf, sizeof(buf), "mot %dC", inverter.motor_temp);
		u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
		u8g2_DrawStr(&u8g2, 200, 40, buf);


        u8g2_SendBuffer(&u8g2);
        osDelay(75);
    }
}

