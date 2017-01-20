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
static void _Curve_Init(void);
static void _LinearCal_Init (void);
static boolean _LinearCal (LinearStruct* a, LinearStruct* b, LinearStruct* c);
static boolean _CurveCal (CurveStruct* axis_1, CurveStruct* axis_2, CurveParam* params);
static void _IsStep(Coord* _current, Coord* _cal);
static void SendPulse (void);

/* Private variables ---------------------------------------------------------*/
static uint8_t GCode_state      = GCODE_WAIT;
static uint8_t GCode_command    = G_NULL;
static Coord Current;
static Coord Target;
static Coord Cal;
static LinearStruct LinearStructMax,LinearStruct_1,LinearStruct_2;
static CurveStruct Axis_1, Axis_2;
static real32 dx,dy,dz;
static boolean stepFlag_x, stepFlag_y, stepFlag_z;
static CurveParam Cur_Params;
real32 Cir_R;
real32 RelativeI, RelativeJ, RelativeK;


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
   
    GCode_state = GCODE_IN_PROGRESS;
    //END STUB
    
    switch (GCode_state)
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
            GCode_state = GCODE_WAIT;
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
    GCode_command = G_02;
    // END STUB
    
    switch (GCode_command)
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
            GCode_command = G_NULL;
            break;
    }
    return ret;
}

static void _LinearCal_Init (void)
{
    dx = Target.x-Current.x;
	dy = Target.y-Current.y;
	dz = Target.z-Current.z;

	if ((fabs(dx) >= fabs(dy)) && (fabs(dx) >= fabs(dz)))
	{
		LinearStructMax.current = &Current.x;
		LinearStructMax.target  = &Target.x;
		LinearStructMax.cal		= &Cal.x;
		LinearStructMax.delta	= Target.x - Current.x;

		LinearStruct_1.current 	= &Current.y;
		LinearStruct_1.target  	= &Target.y;
		LinearStruct_1.cal		= &Cal.y;
		LinearStruct_1.delta	= Target.y - Current.y;

		LinearStruct_2.current 	= &Current.z;
		LinearStruct_2.target  	= &Target.z;
		LinearStruct_2.cal		= &Cal.z;
		LinearStruct_2.delta	= Target.z - Current.z;
	}
	else if ((fabs(dy) >= fabs(dx)) && (fabs(dy) >= fabs(dz)))
	{
		LinearStructMax.current = &Current.y;
		LinearStructMax.target  = &Target.y;
		LinearStructMax.cal		= &Cal.y;
		LinearStructMax.delta	= Target.y - Current.y;

		LinearStruct_1.current 	= &Current.x;
		LinearStruct_1.target  	= &Target.x;
		LinearStruct_1.cal		= &Cal.x;
		LinearStruct_1.delta	= Target.x - Current.x;

		LinearStruct_2.current 	= &Current.z;
		LinearStruct_2.target  	= &Target.z;
		LinearStruct_2.cal		= &Cal.z;
		LinearStruct_2.delta	= Target.z - Current.z;
	}
	else
	{
		LinearStructMax.current = &Current.z;
		LinearStructMax.target  = &Target.z;
		LinearStructMax.cal		= &Cal.z;
		LinearStructMax.delta	= Target.z - Current.z;

		LinearStruct_1.current 	= &Current.y;
		LinearStruct_1.target  	= &Target.y;
		LinearStruct_1.cal		= &Cal.y;
		LinearStruct_1.delta	= Target.y - Current.y;
		
		LinearStruct_2.current 	= &Current.x;
		LinearStruct_2.target  	= &Target.x;
		LinearStruct_2.cal		= &Cal.x;
		LinearStruct_2.delta	= Target.x - Current.x;
	}
}

/*
NOTE:   fabs(da) is maximum
b->cal, c->cal init = 0 => NOTE to handle the initialize in linear fucntion LinearStruct_1.cal is set to 2*steplength at init
*/
static boolean _LinearCal (LinearStruct* a, LinearStruct* b, LinearStruct* c)
{
	boolean ret = FALSE;

	if ( fabs(*(a->target) - *(a->current)) > (STEP_LENGTH_MM/2) )
	{
		*(a->cal) = STEP_LENGTH_MM;	
		ret = TRUE;

	}
	if ( fabs(*(b->target) - *(b->current)) > (STEP_LENGTH_MM/2) )
	{
		*(b->cal) = *(b->cal) + (b->delta / a->delta)*STEP_LENGTH_MM;
		ret = TRUE;
	}
	if ( fabs(*(c->target) - *(c->current)) > (STEP_LENGTH_MM/2) )
	{
		*(c->cal) = *(c->cal) + (c->delta / a->delta)*STEP_LENGTH_MM;
		ret = TRUE;
	}
    
	return ret;
}

void _Curve_Init(void)
{
	real32 temp_angle;

	Cur_Params.Plane = X_Y_PLANE;

	//Select the current working plane
	switch (Cur_Params.Plane)
	{
		case X_Y_PLANE:
			Axis_1.current = &Current.x;
			Axis_1.target  = &Target.x;
			Axis_1.cal	   = &Cal.x;
			Cur_Params.CenterAxis_1 = Current.x + RelativeI; //Take from I

			Axis_2.current = &Current.y;
			Axis_2.target  = &Target.y;
			Axis_2.cal	   = &Cal.y;
			Cur_Params.CenterAxis_2 = Current.y + RelativeJ; //Take from J
			break;

		case Y_Z_PLANE:
			Axis_1.current = &Current.y;
			Axis_1.target  = &Target.y;
			Axis_1.cal	   = &Cal.y;
			Cur_Params.CenterAxis_1 = Current.y + RelativeJ; //Take from J

			Axis_2.current = &Current.z;
			Axis_2.target  = &Target.z;
			Axis_2.cal	   = &Cal.z;
			Cur_Params.CenterAxis_2 = Current.z + RelativeK; //Take from K
			break;

		case Z_X_PLANE:
			Axis_1.current = &Current.z;
			Axis_1.target  = &Target.z;
			Axis_1.cal	   = &Cal.z;
			Cur_Params.CenterAxis_1 = Current.z + RelativeK; //Take from K

			Axis_2.current = &Current.x;
			Axis_2.target  = &Target.x;
			Axis_2.cal	   = &Cal.x;
			Cur_Params.CenterAxis_2 = Current.x + RelativeI; //Take from I
			break;

		default:
			// Send out some warning
			break;
	}

	temp_angle = atan(fabs(*(Axis_2.current) - Cur_Params.CenterAxis_2) / fabs(*(Axis_1.current) - Cur_Params.CenterAxis_1));
	if ((*(Axis_2.current) - Cur_Params.CenterAxis_2) > 0) // I and IV
	{
		// IV
		if ((*(Axis_1.current) - Cur_Params.CenterAxis_1) < 0)
		{
			Cur_Params.CurrentAngle = 2*PI - temp_angle;
		}
		// I: unchange
		else
		{
			Cur_Params.CurrentAngle = temp_angle;
		}
	}
	else
	{
		// III
		if ((*(Axis_1.current) - Cur_Params.CenterAxis_1) < 0)
		{
			Cur_Params.CurrentAngle = temp_angle + PI;
		}
		//II
		else
		{
			Cur_Params.CurrentAngle = PI - temp_angle;
		}
	}

	temp_angle = atan(fabs(*(Axis_2.target) - Cur_Params.CenterAxis_2) / fabs(*(Axis_1.target) - Cur_Params.CenterAxis_1));
		if ((*(Axis_2.target) - Cur_Params.CenterAxis_2) > 0) // I and IV
		{
			// IV
			if ((*(Axis_1.target) - Cur_Params.CenterAxis_1) < 0)
			{
				Cur_Params.TargetAngle = 2*PI - temp_angle;
			}
			// I: unchange
			else
			{
				Cur_Params.TargetAngle = temp_angle;
			}
		}
		else
		{
			// III
			if ((*(Axis_1.target) - Cur_Params.CenterAxis_1) < 0)
			{
				Cur_Params.TargetAngle = temp_angle + PI;
			}
			//II
			else
			{
				Cur_Params.TargetAngle = PI - temp_angle;
			}
		}

		Cir_R = sqrt((*(Axis_2.current) - Cur_Params.CenterAxis_2) * (*(Axis_2.current) - Cur_Params.CenterAxis_2) + 
					(*(Axis_1.current) - Cur_Params.CenterAxis_1) * (*(Axis_1.current) - Cur_Params.CenterAxis_1));
		Cur_Params.CurveStepLength = STEP_LENGTH_MM / Cir_R;

		if (Cur_Params.ClockWise)
		{
			temp_angle = Cur_Params.TargetAngle - Cur_Params.CurrentAngle;
			if ((temp_angle < 0) || ( fabs(temp_angle) < (Cur_Params.CurveStepLength / 2)) )
			{
				temp_angle += 2*PI;
			}
			else
			{
				//Keep unchanged
			}
			Cur_Params.RunTime = (uint32_t)(temp_angle / Cur_Params.CurveStepLength) + 1;
		}
		else
		{
			temp_angle = Cur_Params.CurrentAngle - Cur_Params.TargetAngle;
			if ((temp_angle < 0) || ( fabs(temp_angle) < (Cur_Params.CurveStepLength / 2)) )
			{
				temp_angle += 2*PI;
			}
			else
			{
				//Keep unchanged
			}
			Cur_Params.RunTime = (uint32_t)(temp_angle / Cur_Params.CurveStepLength) + 1;
		}
}

boolean _CurveCal (CurveStruct* axis_1, CurveStruct* axis_2, CurveParam* params)
{
	boolean ret = FALSE;
	real32 temp_angle;

	if (params->RunTime != 0)
	{
		if (params->ClockWise)
		{
			temp_angle = params->CurrentAngle - params->CurveStepLength;
			if (temp_angle < 0)
			{
				temp_angle += 2*PI;
			}
		}
		else
		{
			temp_angle = params->CurrentAngle + params->CurveStepLength;
			if (temp_angle > 2*PI)
			{
				temp_angle -= 2*PI;
			}
		}

		*(axis_1->cal) = *(axis_1->cal) + Cir_R*(sin(temp_angle) - sin(params->CurrentAngle));
		*(axis_2->cal) = *(axis_2->cal) + Cir_R*(cos(params->CurrentAngle) - cos(temp_angle));

		params->CurrentAngle = temp_angle;
		params->RunTime--;
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
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
