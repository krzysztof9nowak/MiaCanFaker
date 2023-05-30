#ifndef _MIAUI_H_
#define _MIAUI_H_

#include <miagl.h>

#include <stdint.h>

typedef enum {
    MUI_DRIVE,
    MUI_NEUTRAL,
    MUI_REVERSE,
} mui_gear;

typedef enum {
    MUI_STATUS_LEFT_BLINKER_ON = 1,
    MUI_STATUS_RIGHT_BLINKER_ON = 2,
    MUI_STATUS_LEFT_DOOR_OPEN = 4,
    MUI_STATUS_RIGHT_DOOR_OPEN = 8,
    MUI_STATUS_MOTOR_FAN_RUNNING = 16,
} mui_status;

typedef struct mui_state {

    const char* debug_text;         //!< any debug text to draw, may be NULL
    uint16_t capacitor_voltage;     //!< in 0.1v, e.g. 762 = 76.2 V
    uint8_t cell_count;             //!< capacitor_voltage / cell_count = cell voltage
    int16_t motor_current;          //!< motor current in Amps (signed)
    mui_gear gear;                  //!< DRIVE, NEUTRAL or REVERSE
    uint16_t vehicle_speed;         //!< absolute value of vehicle speed in kmph
    mui_status light_status;        //!< bitwise combination of MUI_STATUS_* flags
    uint8_t battery_level;          //!< battery level in percent (0-100%)
    uint16_t estimated_range;       //!< estimated range in kilometers, or 0xFFFF if currently unknown
    int8_t motor_temp;              //!< motor temperature in degrees Celsius
    int8_t inverter_temp;           //!< inverter temperature in degrees Celsius
    uint32_t odometer;              //!< odometer reading in kilometers
    uint32_t trip_meter;            //!< trip meter reading in tens of meters, e.g 124 = 1,24 km

    struct {
        uint32_t elapsed_time_ms;
    } __internal;

} mui_state_t, *mui_state_ptr;

void mui_InitInternalState(mui_state_ptr instance);
void mui_Update(mui_state_ptr instance, uint32_t delta_ms);
void mui_Draw(mui_state_ptr instance, miagl_ptr gl);

#endif
