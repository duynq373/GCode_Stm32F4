/**
  ******************************************************************************
  * File Name          : GCode_Linear.c
  * Description        : GCode Interpreter implementation: Linear calculation 
                         function
  * Author	           : duynq373@gmail.com
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "GCode_Linear.h"



void _LinearCal_Init (void)
{
    real32 dx,dy,dz;
    
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
boolean _LinearCal (LinearStruct* a, LinearStruct* b, LinearStruct* c)
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
