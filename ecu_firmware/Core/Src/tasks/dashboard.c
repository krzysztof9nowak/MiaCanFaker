#include "dashboard.h"
#include "main.h"
#include "../SSD1322_API.h"
//#include "u8g2.h"
#include "cmsis_os.h"
#include <task.h>

#include <miagl-buffer.h>
#include <miagl.h>
#include <miaui.h>

DEFINE_MIAGL_STRUCT(256, 64);

miagl_t miagl;
mui_state_t miaui;
//u8g2_t u8g2;
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern volatile inverter_t inverter;
extern volatile bool run;


uint32_t odometer = 0;

float meters_in_pontiff = 0;
float trip = 0;

int16_t abs(int16_t x){
	if(x < 0) return -x;
	return x;
}

//uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
//{
//	/* STM32 supports HW SPI, Remove unused cases like U8X8_MSG_DELAY_XXX & U8X8_MSG_GPIO_XXX */
//	switch(msg)
//	{
//	case U8X8_MSG_GPIO_AND_DELAY_INIT:
//		/* Insert codes for initialization */
//		break;
//	case U8X8_MSG_DELAY_MILLI:
//		/* ms Delay */
//		osDelay(arg_int);
//		break;
//	case U8X8_MSG_GPIO_CS:
//		/* Insert codes for SS pin control */
//		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, arg_int);
//		break;
//	case U8X8_MSG_GPIO_DC:
//		/* Insert codes for DC pin control */
//		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
//		break;
//	case U8X8_MSG_GPIO_RESET:
//		/* Insert codes for RST pin control */
//		HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, arg_int);
//		break;
//	}
//	return 1;
//}
//
//#define TX_TIMEOUT		100
//uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
//{
//	switch(msg) {
//	case U8X8_MSG_BYTE_SEND:
//		/* Insert codes to transmit data */
//		if(HAL_SPI_Transmit(&hspi1, arg_ptr, arg_int, TX_TIMEOUT) != HAL_OK) return 0;
//		break;
//	case U8X8_MSG_BYTE_INIT:
//		/* Insert codes to begin SPI transmission */
//		break;
//	case U8X8_MSG_BYTE_SET_DC:
//		/* Control DC pin, U8X8_MSG_GPIO_DC will be called */
//		u8x8_gpio_SetDC(u8x8, arg_int);
//		break;
//	case U8X8_MSG_BYTE_START_TRANSFER:
//		/* Select slave, U8X8_MSG_GPIO_CS will be called */
//		u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
//		osDelay(1);
//		break;
//	case U8X8_MSG_BYTE_END_TRANSFER:
//		osDelay(1);
//		/* Insert codes to end SPI transmission */
//		u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
//		break;
//	default:
//		return 0;
//	}
//	return 1;
//}

void miagl_driver_init()
{
  SSD1322_API_init();
}

bool miagl_driver_is_buffer_ready()
{
  return true;
}

void miagl_driver_flush_full_screen(void* buffer, uint16_t size)
{
  //SSD1322_API_set_display_mode(SSD1322_MODE_INVERTED);
  SSD1322_API_set_window(0, 63, 0, 127);
  SSD1322_API_send_buffer(buffer, size);
}

void miagl_driver_flush_part_screen(void* buffer, uint16_t size, 
                                    uint8_t start_x, uint8_t end_x, 
                                    uint8_t start_y, uint8_t end_y)
{
  SSD1322_API_set_window(start_x / 4, end_x / 4, start_y, end_y);
  SSD1322_API_send_buffer(buffer, size);
}

static miagl_driver_t miagl_driver = {
  .fn_init_driver = miagl_driver_init,
  .fn_is_buffer_ready = miagl_driver_is_buffer_ready,
  .fn_flush_full_screen = miagl_driver_flush_full_screen,
  .fn_flush_part_screen = miagl_driver_flush_part_screen,
};


void DashboardTask(void *argument){
    HAL_GPIO_WritePin(OLED_POWER_EN_GPIO_Port, OLED_POWER_EN_Pin, SET);
    mgl_InitLibrary(&miagl, 256, 64, &miagl_driver);

    //u8g2_Setup_ssd1322_nhd_256x64_f(&u8g2, U8G2_R0, u8x8_byte_stm32_hw_spi, u8x8_stm32_gpio_and_delay);

    //u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    //u8g2_SetPowerSave(&u8g2, 0); // wake up display

    // u8g2_DrawLine(&u8g2, 50,50, 100, 50);
    //u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    //u8g2_DrawStr(&u8g2, 70,15, "MIA Electric");
    //u8g2_SetFont(&u8g2, u8g2_font_6x13_mr);
    //u8g2_DrawStr(&u8g2, 40, 40, "Wilczyckie zaklady przemyslowe");
    //u8g2_SendBuffer(&u8g2);

    // Read odometer
    HAL_I2C_Mem_Read(&hi2c1,0b10100001,0,2,&odometer,sizeof(uint32_t),10000);

    osDelay(1000);

    char buf[64];
    static uint8_t test = 0;

    while(1){
        //mui_RenderGlTest(&miagl);
        //mgl_SetPixel(&miagl, test++, 40, test & 0xF);
        //mgl_SetPixel(&miagl, test, 42, test & 0xF);
        //mgl_SetPixel(&miagl, test, test, test & 0xF);
        //u8g2_ClearBuffer(&u8g2);
        const float kmh_per_rpm = 24.0 / 2000.0;
        float kmh = abs((int16_t) inverter.speed) * kmh_per_rpm;
        if (run) {
            //u8g2_SetFont(&u8g2, u8g2_font_7Segments_26x42_mn);
            //snprintf(buf, sizeof(buf), "%hd", (int16_t) kmh);
            //u8g2_DrawStr(&u8g2, 100, 50, buf);


            //snprintf(buf, sizeof(buf), "%d.%dV", (int) (inverter.voltage), (int) (inverter.voltage * 10) % 10);
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 0, 50, buf);


            //snprintf(buf, sizeof(buf), "%dA", (int) (inverter.current));
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 0, 40, buf);

            //snprintf(buf, sizeof(buf), "M %dA", (int) (inverter.motor_current));
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 0, 30, buf);


            //snprintf(buf, sizeof(buf), "%d.%dkm", odometer / 1000, (odometer / 100) % 10);
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 0, 10, buf);

            //snprintf(buf, sizeof(buf), "%dm", (int)trip);
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 0, 20, buf);

            //snprintf(buf, sizeof(buf), "ctr %dC", inverter.controller_temp&0xFF);
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 200, 30, buf);

            //snprintf(buf, sizeof(buf), "mot %dC", inverter.motor_temp);
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 200, 40, buf);
            //snprintf(buf, sizeof(buf), "Bat %d.%dV", (int) (inverter.voltage)/22,(int) (inverter.voltage)*100/22%100 );
            //u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
            //u8g2_DrawStr(&u8g2, 190, 50, buf);
        } else {
            trip = 0;
        }
    static TickType_t time = 0;
        time;
        TickType_t new_time = xTaskGetTickCount();
        meters_in_pontiff+= kmh/3.6f*(new_time-time)/1000;
        trip += kmh/3.6f*(new_time-time)/1000;

        if(meters_in_pontiff > 100.f) {
            meters_in_pontiff -= 100;
            odometer += 100;
            HAL_I2C_Mem_Write(&hi2c1,0b10100000,0,2, (uint8_t*)&odometer,sizeof(uint32_t),100);
        }
        time = new_time;

        miaui.estimated_range = -1;
        miaui.cell_count = 20;
        miaui.capacitor_voltage = inverter.voltage * 10;
        miaui.gear = inverter.forward ? MUI_DRIVE : MUI_REVERSE;
        miaui.inverter_temp = inverter.controller_temp;
        miaui.light_status = 0;
        if (inverter.fan_enabled) {
          miaui.light_status |= MUI_STATUS_MOTOR_FAN_RUNNING;
        }
        miaui.motor_current = inverter.current;
        miaui.motor_temp = inverter.motor_temp;
        miaui.odometer = odometer / 1000;
        miaui.trip_meter = trip / 10;
        miaui.vehicle_speed = kmh;

        mui_Draw(&miaui, &miagl);
        mgl_FlushScreen(&miagl);
        osDelay(75);
    }
}

