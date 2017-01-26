/**
  ******************************************************************************
  * File Name          : GCode_Curve.c
  * Description        : GCode Interpreter implementation: Curve calculation 
                         function
  * Author	           : duynq373@gmail.com
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "GCode_Curve.h"

/* Private variables ---------------------------------------------------------*/
extern CurveStruct Axis_1, Axis_2;
static real32 Cir_R;

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
