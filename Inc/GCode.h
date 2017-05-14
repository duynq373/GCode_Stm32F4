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
extern uint8_t Rx_data[2], Rx_Buffer[50], Transfer_cplt;
extern uint8_t Tx_XON[];
extern uint8_t Tx_XOFF[];

/* Macro defines -------------------------------------------------------------*/

/*UART BUFFER*/
#define     UART_BUFFER_LENGTH      20
/* Function like macro*/
#if (DEBUG == STD_ON)
    #define     PULSE_X_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET)
    #define     PULSE_Y_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET)
    #define     PULSE_Z_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET)

    #define     PULSE_X_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)
    #define     PULSE_Y_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET)
    #define     PULSE_Z_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET)
    #define     DB_PULSES_OFF   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14,     \
                                                                                    GPIO_PIN_RESET)
#endif

/*
    * OUTPUT_TYPE_1:
        X:
            PE7 is positive direction
            PE8 is negative direction
        Y:
            PE9 is positive direction
            PE10 is negative direction
        Z:
            PE11 is positive direction
            PE12 is negative direction

    * OUTPUT_TYPE_2:
        X:
            PE7 is direction
                1: POSITIVE
                0: NEGATIVE
            PE8 is pulse
        Y:
            PE9 is direction
                1: POSITIVE
                0: NEGATIVE
            PE10 is pulse
        Z:
            PE11 is direction
                1: POSITIVE
                0: NEGATIVE
            PE12 is pulse
*/

#if (OUPUT_TYPE_1 == STD_ON)
    #define     PULSE_X_P_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET)
    #define     PULSE_X_N_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET)
    #define     PULSE_Y_P_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET)
    #define     PULSE_Y_N_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET)
    #define     PULSE_Z_P_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET)
    #define     PULSE_Z_N_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET)

    #define     PULSE_X_P_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET)
    #define     PULSE_X_N_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET)
    #define     PULSE_Y_P_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET)
    #define     PULSE_Y_N_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET)
    #define     PULSE_Z_P_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET)
    #define     PULSE_Z_N_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET)

    #define     PULSES_OFF      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|        \
                                               GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET)
#endif

#if (OUPUT_TYPE_2 == STD_ON)
    #define     PULSE_X_P_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_SET)
    #define     PULSE_X_N_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);             \
                                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET)
    #define     PULSE_Y_P_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_SET)
    #define     PULSE_Y_N_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);             \
                                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET)
    #define     PULSE_Z_P_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET)
    #define     PULSE_Z_N_ON    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);            \
                                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET)

    #define     PULSE_X_P_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET)
    #define     PULSE_X_N_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET)
    #define     PULSE_Y_P_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET)
    #define     PULSE_Y_N_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET)
    #define     PULSE_Z_P_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET)
    #define     PULSE_Z_N_OFF   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET)

    #define     PULSES_OFF      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12,      \
                                                                                    GPIO_PIN_RESET)
#endif

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

/* Runtime option*/
//#define     DEFINED_COMMAND_PARSER    //comment for orginal command parser

/* For calibration purpose */
#define     _DelayTime_C       (uint8_t*)0x0807FFF0    
#define     DelayTime_C         *(_DelayTime_C)


#endif  /*__GCODE_H*/

