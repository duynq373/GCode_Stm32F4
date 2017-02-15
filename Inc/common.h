/**
  ******************************************************************************
  * File Name          : common.h
  * Description        : Header file for common configuration
  * Author             : duynq373@gmail.com
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include <math.h> // replace by arm lib for math
#include "arm_math.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H
#define __COMMON_H

/* Data types ---------------------------------------------------------------*/
typedef float           real32;
typedef signed char     boolean;

#define     TRUE    1
#define     FALSE   0

/* Machine configuration*/

#define     STEP_PER_MM     10
#define     INCH_TO_MM      (float)25.4    
#define     STEP_LENGTH_MM  (float)0.001
#define     MAX_SPEED       1               /*Depend on speed of output pulse and threshold of machine*/
#define     PRECISION       (float)0.0001

/*	Set of coordinate (x,y,z)
*/
typedef struct
{
    real32 x;
    real32 y;
    real32 z;
} Coord;


#endif
