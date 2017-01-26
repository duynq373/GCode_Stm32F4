/**
  ******************************************************************************
  * File Name          : GCode_Curve.h
  * Description        : Header file for GCode Interpreter implementation: Curve
  * Author	           : duynq373@gmail.com
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCODE_CURVE_H
#define __GCODE_CURVE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "common.h"


/* Structures ---------------------------------------------------------------*/
typedef enum
{
	X_Y_PLANE = 0,
	Z_X_PLANE,
	Y_Z_PLANE
} CurvePlane;

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

/* Private function prototypes -----------------------------------------------*/
void _Curve_Init(void);
boolean _CurveCal (CurveStruct* axis_1, CurveStruct* axis_2, CurveParam* params);



/* Macro defines -------------------------------------------------------------*/
/* PI */
#define     PI                  (real32)3.1415926536

/* Import variables _---------------------------------------------------------*/
extern CurveParam Cur_Params;
extern Coord Current, Target, Cal;
extern real32 RelativeI, RelativeJ, RelativeK;

#endif
