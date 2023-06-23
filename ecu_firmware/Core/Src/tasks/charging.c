#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"

extern volatile charger_t charger;
extern CAN_HandleTypeDef hcan;
extern volatile bool run;
extern osSemaphoreId_t canSemaphoreHandle;


enum charging_state{
    CHG_OFF =0 ,
    CHG_CONNECTED = 1,
    CHG_CHARGING = 2,
    CHG_DONE = 3,
} chg_state = CHG_OFF;

enum charger_status{
    CH_NOT_READY = 0,
    CH_READY = 1,
    CH_CHARGING = 2,
    CH_DONE = 3,
    CH_SHUTDOWN = 4,
    CH_ERROR = 5,
};

#define CAN_BMS_CHA_ID 0x622
void can_bms_cha(CAN_BMS_CHA_t * frame)
{
    CAN_TxHeaderTypeDef carrier = {0};

    carrier.StdId = CAN_BMS_CHA_ID;
    carrier.DLC = 6;
    uint32_t mailbox;
    osSemaphoreAcquire(canSemaphoreHandle, osWaitForever);

    // HAL_CAN_AddTxMessage(&hcan,&carrier,(uint8_t *)frame, &mailbox);
    osSemaphoreRelease(canSemaphoreHandle);

}

#define EGV_CMD_CHA_ID 0x570
void can_egv_cmd_cha(uint8_t msg){
    CAN_TxHeaderTypeDef carrier = {0};

    carrier.StdId = EGV_CMD_CHA_ID;
    carrier.DLC = 1;
    uint32_t mailbox;
    osSemaphoreAcquire(canSemaphoreHandle, osWaitForever);

    // HAL_CAN_AddTxMessage(&hcan,&carrier,(uint8_t *)&msg, &mailbox);
    osSemaphoreRelease(canSemaphoreHandle);
}

void chargingTask(void *arg){
        #define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
        static CAN_BMS_CHA_t bms_cha = {
            .max_voltage = SWAP_UINT16(8800),
            .charging_current = SWAP_UINT16(350),
            .status = 0,
        };


    while(true){
        osDelay(1000);

        HAL_GPIO_WritePin(LED_CHARGING_GPIO_Port,LED_CHARGING_Pin, charger.status == CHG_CHARGING);

        printf("CHG Status %2d Error %2x Presence %2x\r\n", charger.status, charger.error, charger.presence);
        printf("CHG state %d\r\n", chg_state);

        if(chg_state == CHG_OFF){
            charger.request_battery = false;
            bms_cha.status = 0;
            can_bms_cha(&bms_cha);


            if((charger.presence & 0b10) && !run){ // mains voltage detected
                chg_state = CHG_CONNECTED;
            }
            else if(charger.status != CH_SHUTDOWN){
                can_egv_cmd_cha(0); // shutdown
            }


        } else if(chg_state == CHG_CONNECTED){
            charger.request_battery = true;
            bms_cha.status = 0;
            can_bms_cha(&bms_cha);


            if(run){
                chg_state = CHG_OFF;
                bms_cha.status = 0;
                can_bms_cha(&bms_cha);
            } else if(charger.status == CH_READY){
                chg_state = CHG_CHARGING;
            } else if(!(charger.presence & 0b10)){
                chg_state = CHG_OFF;
            }

        } else if(chg_state == CHG_CHARGING){
            charger.request_battery = true;
            bms_cha.status = 0b11;
            can_bms_cha(&bms_cha);

            if(run){
                chg_state = CHG_OFF;
                bms_cha.status = 0;
                can_bms_cha(&bms_cha);
            } else if(charger.status == CH_DONE){
                chg_state = CHG_OFF;
                bms_cha.status = 0;
                can_bms_cha(&bms_cha);
            } else if(!(charger.presence & 0b10)){
                chg_state = CHG_OFF;
            }
        }
    }
}