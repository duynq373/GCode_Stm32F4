/**
  ******************************************************************************
  * File Name          : GCode.h
  * Description        : Header file for GCode Interpreter implementation
  * Author	           : duynq373@gmail.com
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCODE_H
#define __GCODE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <math.h>


/* Private function prototypes -----------------------------------------------*/
void GCode_Intprtr (void);


/* Data types ---------------------------------------------------------------*/
typedef float           real32;
typedef double          real64;
typedef signed char     boolean;

/* Structures ---------------------------------------------------------------*/
typedef enum
{
	X_Y_PLANE = 0,
	Z_X_PLANE,
	Y_Z_PLANE
} CurvePlane;

typedef struct
{
    real32 x;
    real32 y;
    real32 z;
} Coord;

typedef struct 
{
	real32 *current;
	real32 *target;
	real32 *cal;
	real32 delta;
} LinearStruct;

typedef struct
{
	real32 *current;
	real32 *target;
	real32 *cal;
} CurveStruct;

typedef struct
{
	real32 TargetAngle;
	real32 CurrentAngle;
	real32 CurveStepLength;
	real32 CenterAxis_1;
	real32 CenterAxis_2;
	CurvePlane Plane;
	boolean ClockWise;
	uint32_t RunTime;
} CurveParam;

/* Private variables ---------------------------------------------------------*/

/* Macro defines -------------------------------------------------------------*/
/* Function like macro*/
#define     PULSE_X_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET) 
#define     PULSE_Y_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET) 
#define     PULSE_Z_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET) 
#define     PULSE_X_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET) 
#define     PULSE_Y_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET) 
#define     PULSE_Z_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET) 

/* PI */
#define     PI                  (real32)3.1415926536

#define     TRUE    1
#define     FALSE   0

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

/* Machine configuration*/

#define     STEP_PER_MM     10
#define     INCH_TO_MM      (float)25.4    
#define     STEP_LENGTH_MM  (float)0.001
#define     MAX_SPEED       1               /*Depend on speed of output pulse and threshold of machine*/
#define     PRECISION       (float)0.0001


#endif  /*__GCODE_H*/
