/**
  ******************************************************************************
  * File Name          : GCode_Curve.h
  * Description        : Header file for GCode Interpreter implementation: Curve
  * Author             : duynq373@gmail.com
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCODE_CURVE_H
#define __GCODE_CURVE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "common.h"


/* Structures ---------------------------------------------------------------*/

/* Curve plane enumeration to select the current plane for curve commands
    X_Y_PLANE = 0,
    Z_X_PLANE = 1,
    Y_Z_PLANE = 2
*/
typedef enum
{
    X_Y_PLANE = 0,
    Z_X_PLANE,
    Y_Z_PLANE
} CurvePlane;

/*    Data structure for Curve Calculation:
        real32 *current;    pointer to current coordinate of an axis
        real32 *target;     pointer to target coordinate (the finish one) of an axis
        real32 *cal;        pointer to differene after each calculation
*/
typedef struct
{
    real32 *current;
    real32 *target;
    real32 *cal;
} CurveStruct;

/* Data structure for Curve Calculation parameters:
        real32 TargetAngle;         target angle (the finish one): unit in rad
        real32 CurrentAngle;        current angle: unit in rad
        real32 CurveStepLength;     steplength of curve section
        real32 CenterAxis_1;        the coordinate of the Circle center axis 1
        real32 CenterAxis_2;        the coordinate of the Circle center axis 2
        CurvePlane Plane;           plane for the curve command
        _boolean ClockWise;         direction for the curve:
                                        Clockwise:            TRUE
                                        Counter-Clockwise:    FALSE
        uint32_t RunTime;           total calculation times until done for the current command 
*/
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

/* Import variables _---------------------------------------------------------*/
extern CurveParam Cur_Params;
extern Coord Current, Target, Cal;
extern real32 RelativeI, RelativeJ, RelativeK;

#endif
