/**
  ******************************************************************************
  * File Name          : GCode_Curve.c
  * Description        : GCode Interpreter implementation: Curve calculation 
                         function
  * Author               : duynq373@gmail.com
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "GCode_Curve.h"

/* Private variables ---------------------------------------------------------*/
extern CurveStruct Axis_1, Axis_2;
static real32 Cir_R;

/*
    _Curve_Init(): Initialize the Curve Calculation parameters:
        + Define plane => define Axis 1 and Axis 2
        + Calculate the center coordinates: base on relative value (I,J,K)
        + Calculate the current angle and target angle:
                                ^
                                | Axis 2
                                |_ _ _
                                |   /|
                        (II)    |  / |    (I)
                                | /Alpha
                      ----------|/---|---->    Axis 1
                                |
                        (III)   |     (IV)
                                |

            - Angle is calculated by Alpha = arctan (delta Axis_2/delta Axis_1)
            - If alpha is in II, III, or IV (identified by the sign of delta Axis 1 and delta Axis 2), the alpha is calculated in I and add respctive value (do a little simple math here)
                            
        + Calculate the radius
        + Calculate curve steplength
        + Calculate runtime using the length of required angle: l = R.alpha

*/
void _Curve_Init(void)
{
    real32 temp_angle;

    Cur_Params.Plane = X_Y_PLANE;    // TODO: will be replace by a global variable

    //Select the current working plane
    switch (Cur_Params.Plane)
    {
        case X_Y_PLANE:
            Axis_1.current = &Current.x;
            Axis_1.target  = &Target.x;
            Axis_1.cal     = &Cal.x;
            Cur_Params.CenterAxis_1 = Current.x + RelativeI; //Take from I

            Axis_2.current = &Current.y;
            Axis_2.target  = &Target.y;
            Axis_2.cal     = &Cal.y;
            Cur_Params.CenterAxis_2 = Current.y + RelativeJ; //Take from J
            break;

        case Y_Z_PLANE:
            Axis_1.current = &Current.y;
            Axis_1.target  = &Target.y;
            Axis_1.cal     = &Cal.y;
            Cur_Params.CenterAxis_1 = Current.y + RelativeJ; //Take from J

            Axis_2.current = &Current.z;
            Axis_2.target  = &Target.z;
            Axis_2.cal     = &Cal.z;
            Cur_Params.CenterAxis_2 = Current.z + RelativeK; //Take from K
            break;

        case Z_X_PLANE:
            Axis_1.current = &Current.z;
            Axis_1.target  = &Target.z;
            Axis_1.cal     = &Cal.z;
            Cur_Params.CenterAxis_1 = Current.z + RelativeK; //Take from K

            Axis_2.current = &Current.x;
            Axis_2.target  = &Target.x;
            Axis_2.cal     = &Cal.x;
            Cur_Params.CenterAxis_2 = Current.x + RelativeI; //Take from I
            break;

        default:
            // Send out some warning
            break;
    }

        *(Axis_1.cal ) = (real32)0;
        *(Axis_2.cal ) = (real32)0;

    if ( fabs(*(Axis_1.current) - Cur_Params.CenterAxis_1) < PRECISION)    //Precision
    {
        if (*(Axis_2.current) > Cur_Params.CenterAxis_2)
        {
            Cur_Params.CurrentAngle = PI/2;
        }
        else
        {
            Cur_Params.CurrentAngle = 3*PI/2;
        }

    }
    else if (fabs(*(Axis_2.current) - Cur_Params.CenterAxis_2) < PRECISION)    //Precision
    {
        if (*(Axis_1.current) > Cur_Params.CenterAxis_1)
        {
            Cur_Params.CurrentAngle = 0;
        }
        else
        {
            Cur_Params.CurrentAngle = PI;
        }
    }
    else
    {
        temp_angle = atan(fabs(*(Axis_2.current) - Cur_Params.CenterAxis_2) / fabs(*(Axis_1.current) - Cur_Params.CenterAxis_1));
        if ((*(Axis_2.current) - Cur_Params.CenterAxis_2) > 0) // I and II
        {
            // II
            if ((*(Axis_1.current) - Cur_Params.CenterAxis_1) < 0)
            {
                Cur_Params.CurrentAngle = PI - temp_angle;
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
            //IV
            else
            {
                Cur_Params.CurrentAngle = 2*PI - temp_angle;
            }
        }
    }

    if (fabs(*(Axis_1.target) - Cur_Params.CenterAxis_1) < PRECISION)    //Precision
    {
        if (*(Axis_2.target) > Cur_Params.CenterAxis_2)
        {
            Cur_Params.TargetAngle = PI/2;
        }
        else
        {
            Cur_Params.TargetAngle = 3*PI/2;
        }
    }
    else if (fabs(*(Axis_2.target) - Cur_Params.CenterAxis_2) < PRECISION)    //Precision
    {
        if (*(Axis_1.target) > Cur_Params.CenterAxis_1)
        {
            if (Cur_Params.ClockWise)
            {    
                Cur_Params.TargetAngle = 0;
            }
            else
            {
                Cur_Params.TargetAngle = 2*PI;
            }
        }
        else
        {
            Cur_Params.TargetAngle = PI;
        }
    }
    else 
    {
        temp_angle = atan(fabs(*(Axis_2.target) - Cur_Params.CenterAxis_2) / fabs(*(Axis_1.target) - Cur_Params.CenterAxis_1));
        
        if ((*(Axis_2.target) - Cur_Params.CenterAxis_2) > 0) // I and II
        {
            // II
            if ((*(Axis_1.target) - Cur_Params.CenterAxis_1) < 0)
            {
                Cur_Params.TargetAngle = PI - temp_angle;
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
            //IV
            else
            {
                Cur_Params.TargetAngle = 2*PI - temp_angle;
            }
        }
    }
    Cir_R = sqrt((*(Axis_2.current) - Cur_Params.CenterAxis_2) * (*(Axis_2.current) - Cur_Params.CenterAxis_2) + 
                (*(Axis_1.current) - Cur_Params.CenterAxis_1) * (*(Axis_1.current) - Cur_Params.CenterAxis_1));
    Cur_Params.CurveStepLength = STEP_LENGTH_MM / Cir_R;

    

    temp_angle = Cur_Params.TargetAngle - Cur_Params.CurrentAngle;
    if (Cur_Params.ClockWise)
    {
        temp_angle = -temp_angle;
    }
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

/*
    _CurveCal(): Calculate the difference between each loop for each axis
        Axis 1 = R.cosine(delta_alpha)
        Axis 2 = R.sine(delta_alpha)

*/
boolean _CurveCal (CurveStruct* axis_1, CurveStruct* axis_2, CurveParam* params)
{
    boolean ret = FALSE;
    real32 temp_angle;

    if (params->RunTime != 0)
    {
        if (params->ClockWise)
        {
            temp_angle = params->CurrentAngle + params->CurveStepLength;
            if (temp_angle < 0)
            {
                temp_angle += 2*PI;
            }
        }
        else
        {
            temp_angle = params->CurrentAngle - params->CurveStepLength;
            if (temp_angle > 2*PI)
            {
                temp_angle -= 2*PI;
            }
        }
        *(axis_1->cal) = *(axis_1->cal) + Cir_R*(arm_cos_f32(temp_angle) - arm_cos_f32(params->CurrentAngle));
        *(axis_2->cal) = *(axis_2->cal) + Cir_R*(arm_sin_f32(params->CurrentAngle) - arm_sin_f32(temp_angle));    
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
