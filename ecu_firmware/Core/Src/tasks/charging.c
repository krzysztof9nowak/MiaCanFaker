#include "main.h"
#include "cmsis_os.h"

void chargingTask(void *arg){
    while(true){
        osDelay(1000);
    }
}