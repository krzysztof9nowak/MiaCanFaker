#pragma once

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define CAN_EGV_ACCEL_VAR_ID (0x201)
#define CAN_EGV_CMD_VAR_ID (0x301)
#define CAN_EGV_SYNC_ALL_ID (0x80)
#define INVERTER_STATUS_OK (1075)

typedef enum{
    INVERTER_STATUS_OFF = 0,
    INVERTER_STATUS_INIT,
    INVERTER_STATUS_RUN,
    INVERTER_STATUS_ERROR
} inverter_status_t;
typedef struct{
    inverter_status_t status;
    uint16_t throttle;
    uint16_t status_word;
    uint16_t speed;
    bool forward;
    bool enabled;
} inverter_t;

typedef struct __attribute__((__packed__))
{
    int32_t motor_speed;
    int16_t motor_torque;
    int16_t status_word;
} VAR_Stat1_EGV_t;
typedef struct  __attribute__((__packed__))
{
    uint16_t accelerator_set_point;//little endian
    uint16_t regen_max;
    unsigned int forward : 1 ;
    unsigned int reverse : 1 ;
    unsigned int footbrake : 1;
    unsigned int DS1 : 1 ;
    unsigned int footswitch :1;
    unsigned int DS2 : 1;
    unsigned int unused :2;

} CAN_EGV_Accel_VAR_t;

typedef struct __attribute__((__packed__))
{
    int16_t current_limit; //0 to 500
    int16_t regen_limit;
    uint16_t max_torque_ratio;
    uint16_t motor_command;

} CAN_EGV_Cmd_VAR_t;

typedef struct __attribute__((__packed__))
{
    int16_t charging_current; //0 to 500
    int16_t max_voltage;
    uint8_t status;
} CAN_BMS_CHA_t;



typedef struct CAN_EGV_SYNC_ALL
{
    uint8_t status;
} CAN_EGV_SYNC_ALL_t;
