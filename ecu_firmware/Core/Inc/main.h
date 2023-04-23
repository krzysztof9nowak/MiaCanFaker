/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sevcon.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_INDICATOR_Pin GPIO_PIN_2
#define LED_INDICATOR_GPIO_Port GPIOE
#define LED_AIRBAG_Pin GPIO_PIN_3
#define LED_AIRBAG_GPIO_Port GPIOE
#define LED_ABS_Pin GPIO_PIN_4
#define LED_ABS_GPIO_Port GPIOE
#define LED_BELT_Pin GPIO_PIN_5
#define LED_BELT_GPIO_Port GPIOE
#define LED_BRAKE_Pin GPIO_PIN_6
#define LED_BRAKE_GPIO_Port GPIOE
#define LED_STOP_Pin GPIO_PIN_13
#define LED_STOP_GPIO_Port GPIOC
#define LED_ELECTR_Pin GPIO_PIN_14
#define LED_ELECTR_GPIO_Port GPIOC
#define LED_TEMP_Pin GPIO_PIN_15
#define LED_TEMP_GPIO_Port GPIOC
#define LIGHT_DAYTIME_Pin GPIO_PIN_1
#define LIGHT_DAYTIME_GPIO_Port GPIOC
#define APC_Pin GPIO_PIN_2
#define APC_GPIO_Port GPIOC
#define INVERTER_Pin GPIO_PIN_3
#define INVERTER_GPIO_Port GPIOC
#define VBAT_Pin GPIO_PIN_0
#define VBAT_GPIO_Port GPIOA
#define THR1_Pin GPIO_PIN_1
#define THR1_GPIO_Port GPIOA
#define THR2_Pin GPIO_PIN_2
#define THR2_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_3
#define OLED_CS_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_4
#define OLED_DC_GPIO_Port GPIOA
#define OLED_RES_Pin GPIO_PIN_6
#define OLED_RES_GPIO_Port GPIOA
#define WINDSHIELD_HEAT_Pin GPIO_PIN_4
#define WINDSHIELD_HEAT_GPIO_Port GPIOC
#define WIPERS_Pin GPIO_PIN_5
#define WIPERS_GPIO_Port GPIOC
#define BMS_Pin GPIO_PIN_0
#define BMS_GPIO_Port GPIOB
#define INDIC_LEFT_Pin GPIO_PIN_1
#define INDIC_LEFT_GPIO_Port GPIOB
#define INDIC_RIGHT_Pin GPIO_PIN_2
#define INDIC_RIGHT_GPIO_Port GPIOB
#define LIGHT_STOP_Pin GPIO_PIN_7
#define LIGHT_STOP_GPIO_Port GPIOE
#define LIGHT_REVERSE_Pin GPIO_PIN_8
#define LIGHT_REVERSE_GPIO_Port GPIOE
#define HEATING_Pin GPIO_PIN_9
#define HEATING_GPIO_Port GPIOE
#define LED_CHARGING_Pin GPIO_PIN_10
#define LED_CHARGING_GPIO_Port GPIOE
#define LED_BATTERY_HV_Pin GPIO_PIN_11
#define LED_BATTERY_HV_GPIO_Port GPIOE
#define OLED_POWER_EN_Pin GPIO_PIN_12
#define OLED_POWER_EN_GPIO_Port GPIOE
#define IN_DISP_Pin GPIO_PIN_13
#define IN_DISP_GPIO_Port GPIOE
#define IN_DISP_EXTI_IRQn EXTI15_10_IRQn
#define IN_CEINT_Pin GPIO_PIN_14
#define IN_CEINT_GPIO_Port GPIOE
#define IN_CTL_APC_Pin GPIO_PIN_15
#define IN_CTL_APC_GPIO_Port GPIOE
#define IN_ECO_Pin GPIO_PIN_10
#define IN_ECO_GPIO_Port GPIOB
#define IN_WINDSHIELD_HEAT_Pin GPIO_PIN_11
#define IN_WINDSHIELD_HEAT_GPIO_Port GPIOB
#define IN_DRIVE_DIR_1_Pin GPIO_PIN_12
#define IN_DRIVE_DIR_1_GPIO_Port GPIOB
#define IN_WIPER_Pin GPIO_PIN_13
#define IN_WIPER_GPIO_Port GPIOB
#define IN_FOG_LIGHT_Pin GPIO_PIN_14
#define IN_FOG_LIGHT_GPIO_Port GPIOB
#define IN_SIDELIGHT_Pin GPIO_PIN_15
#define IN_SIDELIGHT_GPIO_Port GPIOB
#define IN_BRAKE_1_Pin GPIO_PIN_8
#define IN_BRAKE_1_GPIO_Port GPIOD
#define IN_INDICATOR_RIGHT_Pin GPIO_PIN_9
#define IN_INDICATOR_RIGHT_GPIO_Port GPIOD
#define LED_SIDELIGHTS_Pin GPIO_PIN_10
#define LED_SIDELIGHTS_GPIO_Port GPIOD
#define LED_HEADLIGHT_Pin GPIO_PIN_11
#define LED_HEADLIGHT_GPIO_Port GPIOD
#define IN_WINDSHIELD_JET_Pin GPIO_PIN_2
#define IN_WINDSHIELD_JET_GPIO_Port GPIOD
#define IN_DRIVE_DIR_2_Pin GPIO_PIN_3
#define IN_DRIVE_DIR_2_GPIO_Port GPIOD
#define IN_DF_CONV_Pin GPIO_PIN_4
#define IN_DF_CONV_GPIO_Port GPIOD
#define IN_PARK_Pin GPIO_PIN_5
#define IN_PARK_GPIO_Port GPIOD
#define IN_AIRBAG_Pin GPIO_PIN_6
#define IN_AIRBAG_GPIO_Port GPIOD
#define IN_KEY2_Pin GPIO_PIN_7
#define IN_KEY2_GPIO_Port GPIOD
#define IN_CRASH_Pin GPIO_PIN_3
#define IN_CRASH_GPIO_Port GPIOB
#define IN_KEY1_Pin GPIO_PIN_4
#define IN_KEY1_GPIO_Port GPIOB
#define IN_HIGHBEAM_Pin GPIO_PIN_5
#define IN_HIGHBEAM_GPIO_Port GPIOB
#define LED_HEATER_Pin GPIO_PIN_9
#define LED_HEATER_GPIO_Port GPIOB
#define LED_FOG_Pin GPIO_PIN_0
#define LED_FOG_GPIO_Port GPIOE
#define LED_BATTERY_Pin GPIO_PIN_1
#define LED_BATTERY_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
