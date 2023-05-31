#include "dashboard.h"
#include "main.h"
#include "cmsis_os.h"
#include <task.h>

#include "../SSD1322_API.h"

#include <miagl-buffer.h>
#include <miagl.h>
#include <miaui.h>

DEFINE_MIAGL_STRUCT(256, 64);

miagl_t miagl;
mui_state_t miaui;
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern volatile inverter_t inverter;
extern volatile bool run;


uint32_t odometer = 0;

float meters_in_pontiff = 0;
float trip = 0;

static inline int16_t abs(int16_t x){
	if(x < 0) return -x;
	return x;
}

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

    // Read odometer
    HAL_I2C_Mem_Read(&hi2c1,0b10100001,0,2,&odometer,sizeof(uint32_t),10000);

    osDelay(1000);

    char buf[64];
    static uint8_t test = 0;

    while (1) {
        const float kmh_per_rpm = 24.0 / 2000.0;
        float kmh = abs((int16_t) inverter.speed) * kmh_per_rpm;
        if (run) {
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
        } else {
            trip = 0;
        }

        static TickType_t time = 0;
        TickType_t new_time = xTaskGetTickCount();
        meters_in_pontiff+= kmh/3.6f*(new_time-time)/1000;
        trip += kmh/3.6f*(new_time-time)/1000;

        if(meters_in_pontiff > 100.f) {
            meters_in_pontiff -= 100;
            odometer += 100;
            HAL_I2C_Mem_Write(&hi2c1,0b10100000,0,2, (uint8_t*)&odometer,sizeof(uint32_t),100);
        }
        time = new_time;

        mgl_FlushScreen(&miagl);
        osDelay(75);
    }
}

