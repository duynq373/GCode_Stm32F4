/**
  ******************************************************************************
  * File Name          : GCode.c
  * Description        : GCode Interpreter implementation
  * Author               : duynq373@gmail.com
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

real32 steplength_x = (real32)0.01;       /*    steplength for X axis*/
real32 steplength_y = (real32)0.1;        /*    steplength for Y axis*/
real32 steplength_z = (real32)0.1;        /*    steplength for Z axis*/

static uint8_t GCode_State      = GCODE_WAIT;
static uint8_t GCode_Command    = G_NULL;
static boolean InitFlag = FALSE;
Coord Current, Target, Cal;
    /*Curve Calculation*/
CurveStruct Axis_1, Axis_2;
CurveParam Cur_Params;
real32 RelativeI, RelativeJ, RelativeK;    /* relative values of x, y, z to indentify the center in curve command*/
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
    RelativeI = -50;
    RelativeJ = -50;
    RelativeK = 1;

    Cur_Params.ClockWise = FALSE;

    Cal.x = (real32)0;
    Cal.y = (real32)0;
    Cal.z = (real32)0;

    Current.x = (real32)50;
    Current.y = (real32)50;
    Current.z = (real32)0;

    Target.x = (real32)50;
    Target.y = (real32)50;
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
    InitFlag = TRUE;
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
            if (InitFlag)
            {
                _LinearCal_Init();
                InitFlag = FALSE;
            }
            ret  = _LinearCal(&LinearStructMax, &LinearStruct_1, &LinearStruct_2);
            break;
        
        case G_02:
        case G_03:
            //Code here
            if (InitFlag)
            {
                _Curve_Init();
                InitFlag = FALSE;
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

    if ( (fabs(_cal->x) > steplength_x) || ( fabs(fabs(_cal->x) - steplength_x) < PRECISION))
    {
        //One is positive pulse, one is negative pulse
        if (_cal->x > 0)
        {
            _current->x -= steplength_x;
            _cal->x -= steplength_x;
        }
        else
        {
            _current->x += steplength_x;
            _cal->x += steplength_x;
        }
        
        //set 1 for output flag
        stepFlag_x = TRUE;
    }
    
    if ( (fabs(_cal->y) > steplength_y) || ( fabs(fabs(_cal->y) - steplength_y) < PRECISION))
    {
        if (_cal->y > 0)
        {
            _current->y -= steplength_y;
            _cal->y -= steplength_y;
        }
        else
        {
            _current->y += steplength_y;
            _cal->y += steplength_y;
        }
        
        //set 1 for output flag
        stepFlag_y = TRUE;
    }

    if ( (fabs(_cal->z) > steplength_z) || ( fabs(fabs(_cal->z) - steplength_z) < PRECISION))
    {
        if (_cal->z > 0)
        {
            _current->z -= steplength_z;
            _cal->z -= steplength_z;
        }
        else
        {
            _current->z += steplength_z;
            _cal->z += steplength_z;
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
