/**
  ******************************************************************************
  * File Name          : GCode.h
  * Description        : Header file for GCode Interpreter implementation
  * Author             : duynq373@gmail.com
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCODE_H
#define __GCODE_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "GCode_Linear.h"
#include "GCode_Curve.h"


/* Private function prototypes -----------------------------------------------*/
void GCode_Intprtr (void);


/* Structures ---------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/

/* Import variables ----------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

/* Macro defines -------------------------------------------------------------*/

/*UART BUFFER*/
#define     UART_BUFFER_LENGTH      38
/* Function like macro*/
#define     PULSE_X_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET) 
#define     PULSE_Y_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET) 
#define     PULSE_Z_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET) 
#define     PULSE_X_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET) 
#define     PULSE_Y_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET) 
#define     PULSE_Z_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET) 
#define     PULSES_OFF      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET) 

/* State machine states*/
#define     GCODE_WAIT          0x01
#define     GCODE_IN_PROGRESS   0x02
#define     GCODE_DONE          0x03

/* GCode*/
#define     G_00     0x00
#define     G_01     0x01
#define     G_02     0x02
#define     G_03     0x03
#define     G_NULL   0xFF

#endif  /*__GCODE_H*/
