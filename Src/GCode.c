/**
  ******************************************************************************
  * File Name          : GCode.c
  * Description        : GCode Interpreter implementation
  * Author	           : duynq373@gmail.com
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "GCode.h"
  
/* Private function prototypes -----------------------------------------------*/
static void D_InProgress (void);
static boolean _CalcNextCoord (void);
static void _IsStep(Coord* _current, Coord* _cal);
static void SendPulse (void);

/* Private variables ---------------------------------------------------------*/
static uint8_t GCode_State      = GCODE_WAIT;
static uint8_t GCode_Command    = G_NULL;
Coord Current, Target, Cal;
    /*Curve Calculation*/
CurveStruct Axis_1, Axis_2;
CurveParam Cur_Params;
real32 RelativeI, RelativeJ, RelativeK;
    /*Linear Calculation*/
LinearStruct LinearStructMax,LinearStruct_1,LinearStruct_2;

static boolean stepFlag_x, stepFlag_y, stepFlag_z;

/*  Interpreter state machine:
    - WAIT: wait for GCODE command from PC via UART
    - IN PROGRESS: interprete the code and run
    - DONE: Inform PC and re-init system for next command
*/
void GCode_Intprtr (void)
{
    //STUB for read command from PC
    //For Linear
    // Cal.x = (real32)0;
	// Cal.y = (real32)0;
	// Cal.z = (real32)0;

	// Current.x = (real32)0;
	// Current.y = (real32)4;
	// Current.z = (real32)1;

	// Target.x = (real32)100;
	// Target.y = (real32)4;
	// Target.z = (real32)1;
    
    //For Curve
    RelativeI = -30;
	RelativeJ = -30;
	RelativeK = 1;

	Cur_Params.ClockWise = FALSE;

	Cal.x = (real32)0;
	Cal.y = (real32)0;
	Cal.z = (real32)0;

	Current.x = (real32)30;
	Current.y = (real32)30;
	Current.z = (real32)0;

	Target.x = (real32)30;
	Target.y = (real32)30;
	Target.z = (real32)0;
   
    GCode_State = GCODE_IN_PROGRESS;
    //END STUB
    
    switch (GCode_State)
    {
        case GCODE_WAIT:
            //code here
            break;
        
        case GCODE_IN_PROGRESS:
            //code here
            D_InProgress();
            break;
        
        case GCODE_DONE:
            //code here
            break;
        
        default:
            GCode_State = GCODE_WAIT;
            break;
    }
}

/*
    3 steps here:
    - Calculate the next coordinator: _CalcNextCoord()
    - Check whether or not it require a step: _IsStep() for x,y,z maybe implement in send out signal << if (_IsStep (x)) ...
    - Send signal to output: _SendPulse()
    
*/
static void D_InProgress (void)
{
    while (_CalcNextCoord())
    {
        _IsStep(&Current, &Cal);
        SendPulse();
    }
}

/*
    Calculate the next Coordinate for movement required command G00, G01, G02, G03, etc
    @Input:
        + GCode type: linear or curve or others
        + Target coordinate value
            
*/
static boolean _CalcNextCoord (void)
{
    boolean ret = FALSE;
    // STUB
    GCode_Command = G_02;
    // END STUB
    
    switch (GCode_Command)
    {
        case G_00:
            //Code here
            break;
        
        case G_01:
            //Code here
            if ((LinearStructMax.cal == NULL) || (fabs(*(LinearStructMax.cal)) > STEP_LENGTH_MM))
            {
                _LinearCal_Init();
            }
            ret  = _LinearCal(&LinearStructMax, &LinearStruct_1, &LinearStruct_2);
            break;
        
        case G_02:
        case G_03:
            //Code here
            if ((Axis_1.cal == NULL) || (fabs(*(Axis_1.cal)) > STEP_LENGTH_MM))
            {
                _Curve_Init();
            }
            ret  = _CurveCal(&Axis_1, &Axis_2, &Cur_Params); 
            break;
        
        default:
            GCode_Command = G_NULL;
            break;
    }
    return ret;
}




static void _IsStep (Coord* _current, Coord* _cal)
{

	if ( (fabs(_cal->x) > STEP_LENGTH_MM) || ( fabs(fabs(_cal->x) - STEP_LENGTH_MM) < PRECISION))
	{
		if (_cal->x > 0)
		{
			_current->x -= STEP_LENGTH_MM;
			_cal->x -= STEP_LENGTH_MM;
		}
		else
		{
			_current->x += STEP_LENGTH_MM;
			_cal->x += STEP_LENGTH_MM;
		}
		
		//set 1 for output flag
        stepFlag_x = TRUE;
	}

	if ( (fabs(_cal->y) > STEP_LENGTH_MM) || ( fabs(fabs(_cal->y) - STEP_LENGTH_MM) < PRECISION))
	{
		if (_cal->y > 0)
		{
			_current->y -= STEP_LENGTH_MM;
			_cal->y -= STEP_LENGTH_MM;
		}
		else
		{
			_current->y += STEP_LENGTH_MM;
			_cal->y += STEP_LENGTH_MM;
		}
		//set 1 for output flag
        stepFlag_y = TRUE;
	}
	if ( (fabs(_cal->z) > STEP_LENGTH_MM) || ( fabs(fabs(_cal->z) - STEP_LENGTH_MM) < PRECISION))
	{
		if (_cal->z > 0)
		{
			_current->z -= STEP_LENGTH_MM;
			_cal->z -= STEP_LENGTH_MM;
		}
		else
		{
			_current->z += STEP_LENGTH_MM;
			_cal->z += STEP_LENGTH_MM;
		}
		//set 1 for output flag
        stepFlag_z = TRUE;
	}
}

static void SendPulse (void)
{
    // Get speed parameter from PC and calculate the delay time
//    uint16_t delay_time = 1;
    //Check again for positive pulse and negative pulse
    
    if (stepFlag_x == TRUE)
    {
        PULSE_X_ON;
        PULSE_X_ON;
        PULSE_X_ON;
    }
    
    if (stepFlag_y == TRUE)
    {
        PULSE_Y_ON;
        PULSE_Y_ON;
        PULSE_Y_ON;
    }
    if (stepFlag_z == TRUE)
    {
        PULSE_Z_ON;
        PULSE_Z_ON;
        PULSE_Z_ON;
    }
    
//    HAL_Delay(delay_time);
    PULSE_X_OFF;
    PULSE_X_OFF;
    PULSE_X_OFF;
    PULSE_Y_OFF;
    PULSE_Y_OFF;
    PULSE_Y_OFF;
    PULSE_Z_OFF;
    PULSE_Z_OFF;
    PULSE_Z_OFF;
//    HAL_Delay(delay_time);
        
    // Reinit the step flags
    stepFlag_x = FALSE;
    stepFlag_y = FALSE;
    stepFlag_z = FALSE;
}
