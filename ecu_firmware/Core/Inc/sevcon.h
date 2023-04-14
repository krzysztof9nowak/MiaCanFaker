#pragma once

#include "main.h"
//#include "cmsis_os.h"

uint16_t swap_endianness(uint16_t value);

#define CAN_EGV_ACCEL_VAR_ID 0x201
#define CAN_EGV_CMD_VAR_ID 0x301
#define CAN_EGV_SYNC_ALL_ID 0x80


volatile uint8_t var_ready;

typedef struct CAN_EGV_Accel_VAR
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

typedef struct CAN_EGV_Cmd_VAR
{
    int16_t current_limit; //0 to 500
    int16_t regen_limit;
    uint16_t max_torque_ratio;
    uint16_t motor_command;

} CAN_EGV_Cmd_VAR_t;

typedef struct CAN_EGV_SYNC_ALL
{
    int bms : 1;
    int var :1;
    int abs :1;
    int immo :1;
    int charger :1;
    int bvs:1;
    int unused:1;
    int diag:1;

} CAN_EGV_SYNC_ALL_t;

void can_send_egv_sync_all(CAN_EGV_SYNC_ALL_t * frame);

void can_send_egv_accel_var(CAN_EGV_Accel_VAR_t * frame);


void can_send_egv_cmd_var(CAN_EGV_Cmd_VAR_t * frame);


uint16_t get_throttle();


void update_accel_pedal(CAN_EGV_Accel_VAR_t * frame);
