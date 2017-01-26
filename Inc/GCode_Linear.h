/**
  ******************************************************************************
  * File Name          : GCode_Linear.h
  * Description        : Header file for GCode Interpreter implementation: Linear
  * Author	           : duynq373@gmail.com
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCODE_LINEAR_H
#define __GCODE_LINEAR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "common.h"


/* Structures ---------------------------------------------------------------*/
typedef struct 
{
	real32 *current;
	real32 *target;
	real32 *cal;
	real32 delta;
} LinearStruct;


/* Private function prototypes -----------------------------------------------*/
void _LinearCal_Init (void);
boolean _LinearCal (LinearStruct* a, LinearStruct* b, LinearStruct* c);

/* Import variables ----------------------------------------------------------*/
extern Coord Current, Target, Cal;
extern LinearStruct LinearStructMax,LinearStruct_1,LinearStruct_2;

#endif
