/**
  ******************************************************************************
  * File Name          : GCode.c
  * Description        : GCode Interpreter implementation
  * Author               : duynq373@gmail.com
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "GCode.h"
#include "stdlib.h"

  
/* Private function prototypes -----------------------------------------------*/
static void D_InProgress (void);
static boolean _IsNewCommandReceived(void);
static void _CalcNextCoord (void);
static void _IsStep(Coord* _current, Coord* _cal);
static void SendPulse (void);
#ifdef  DEFINED_COMMAND_PARSER
static real32 FloatExtract (uint8_t index, uint8_t* data);
#endif

/* Private variables ---------------------------------------------------------*/

real32 steplength_x = (real32)0.005;       /*    steplength for X axis*/
real32 steplength_y = (real32)0.005;        /*    steplength for Y axis*/
real32 steplength_z = (real32)0.005;        /*    steplength for Z axis*/

static uint8_t GCode_State      = GCODE_WAIT;
static uint8_t GCode_Command    = G_NULL;
//static boolean InitFlag = FALSE;
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
    // RelativeI = -50;
    // RelativeJ = -50;
    // RelativeK = 1;

    // Cur_Params.ClockWise = FALSE;

    // Cal.x = (real32)0;
    // Cal.y = (real32)0;
    // Cal.z = (real32)0;

    // Current.x = (real32)50;
    // Current.y = (real32)50;
    // Current.z = (real32)0;

    // Target.x = (real32)50;
    // Target.y = (real32)50;
    // Target.z = (real32)0;
   
    // GCode_State = GCODE_IN_PROGRESS;
    //END STUB
    //uint8_t trm_bf = 1;
    uint8_t retry_f = 0;
    switch (GCode_State)
    {
        case GCODE_WAIT:
            //code here
            //wait and receive params
            if (_IsNewCommandReceived())
            {
                GCode_State = GCODE_IN_PROGRESS;
            }
            //else
            //{
                /*Do nothing*/
                /* GCode_State = GCODE_WAIT;*/
            //}
            break;
        
        case GCODE_IN_PROGRESS:
            //code here
            D_InProgress();
            GCode_State = GCODE_DONE;
            break;
        
        case GCODE_DONE:
            //code here
            // send feedback to PC: use while here to ensure the Tx_ON is sent properly
			while (retry_f < 3)
			{
				if(HAL_OK != HAL_UART_Transmit_IT(&huart2,Tx_XON,1))
				{
					HAL_Delay(10);
				}
				else 
				{
					break;
				}
				retry_f++;
			}
			
			HAL_UART_Receive_IT(&huart2, Rx_data, 1);	//activate UART receive interrupt every time
            GCode_State = GCODE_WAIT;
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
//    InitFlag = TRUE;
    _CalcNextCoord();
}

/*
    Calculate the next Coordinate for movement required command G00, G01, G02, G03, etc
    @Input:
        + GCode type: linear or curve or others
        + Target coordinate value
            
*/
static void _CalcNextCoord (void)
{
    //boolean ret = FALSE;
    // STUB
    //GCode_Command = G_02;
    // END STUB
    
    switch (GCode_Command)
    {
        case G_00:
            //Code here
            //break;  // Implement later, now using the same algorithm as G_01
        
        case G_01:
            //Code here
            _LinearCal_Init();
            while (_LinearCal(&LinearStructMax, &LinearStruct_1, &LinearStruct_2))
            {
                _IsStep(&Current, &Cal);
                SendPulse();
            }
            break;
        
        case G_02:
        case G_03:
            //Code here
            _Curve_Init();
            while (_CurveCal(&Axis_1, &Axis_2, &Cur_Params))
            {
                _IsStep(&Current, &Cal);
                SendPulse();
            }
            break;
        
        default:
            GCode_Command = G_NULL;
            break;
    }
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
    volatile uint32_t temp = 0;
    if (stepFlag_x == TRUE)
    {
        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
//        PULSE_X_ON;
    }
    
    if (stepFlag_y == TRUE)
    {
        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
    }
    if (stepFlag_z == TRUE)
    {
        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
    }
    
    while (temp < 50)
    {
        temp++;
    }
    temp = 0;
//    HAL_Delay(delay_time);
    PULSE_X_OFF;
//    PULSE_X_OFF;
//    PULSE_X_OFF;
//    PULSE_Y_OFF;
//    PULSE_Y_OFF;
    PULSE_Y_OFF;
    PULSE_Z_OFF;
//    PULSE_Z_OFF;
//    PULSE_Z_OFF;
//    HAL_Delay(delay_time);
        
    // Reinit the step flags
    stepFlag_x = FALSE;
    stepFlag_y = FALSE;
    stepFlag_z = FALSE;
}


static boolean _IsNewCommandReceived(void)
{
    boolean ret = FALSE;
    #ifdef  DEFINED_COMMAND_PARSER
	uint8_t rcv_bf[UART_BUFFER_LENGTH];
    uint32_t temp = 0;
    #else
    char *strm;
	uint8_t str_index = 0;
	uint8_t temp = 0;
    uint8_t i;
    #endif
    
    if (Transfer_cplt == 1)
    {
		while (HAL_OK != HAL_UART_Transmit_IT(&huart2,Tx_XOFF,1))
		{;}
		Transfer_cplt = 0;
    #ifdef  DEFINED_COMMAND_PARSER
        /*Calculate the command*/
        temp = (rcv_bf[0] - 0x30) *10 + (rcv_bf[1] - 0x30);     // temporary solution for hex and ascii
        switch(temp)
        {
            case G_00:
                GCode_Command = G_00;
            break;
            
            case G_01:
                GCode_Command = G_01;
            break;
            
            case G_02:
                GCode_Command = G_02;
                Cur_Params.ClockWise = TRUE;
            break;
            
            case G_03:
                GCode_Command = G_03;
                Cur_Params.ClockWise = FALSE;
            break;
            
            default:
                GCode_Command = G_NULL;
            break;
        }
        
        /*Calculate the Target Coordinates*/
        Target.x = FloatExtract(2, rcv_bf);
        Target.y = FloatExtract(8, rcv_bf);
        Target.z = FloatExtract(14, rcv_bf);
                   
        /*Calculate the Relative values*/
        if ((GCode_Command == G_02) || (GCode_Command == G_03))
        {
            RelativeI = FloatExtract(20, rcv_bf);
            RelativeJ = FloatExtract(26, rcv_bf);
            RelativeK = FloatExtract(32, rcv_bf);
        }
        
        ret = TRUE;
    #else
		if (Rx_Buffer[str_index] != '(')
		{
	        /*G parsing*/
	        if (Rx_Buffer[str_index] == 'G')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < 3; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	                
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                GCode_Command = atoi (strm);
	                free(strm);
	            }
	        }
	        if (Rx_Buffer[str_index] == ' ')
	        {
	            str_index++;
	        }
	        /*X parsing*/
	        if (Rx_Buffer[str_index] == 'X')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < str_index + 10; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                Target.x = atof (strm);
	                free(strm);
	            }
	        }
	        if (Rx_Buffer[str_index] == ' ')
	        {
	            str_index++;
	        }
	        /*Y parsing*/
	        if (Rx_Buffer[str_index] == 'Y')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < str_index + 10; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                Target.y = atof (strm);
	                free(strm);
	            }
	        }
	        if (Rx_Buffer[str_index] == ' ')
	        {
	            str_index++;
	        }
	        /*Z parsing*/
	        if (Rx_Buffer[str_index] == 'Z')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < str_index + 10; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                Target.z = atof (strm);
	                free(strm);
	            }
	        }
	        if (Rx_Buffer[str_index] == ' ')
	        {
	            str_index++;
	        }
	        /*I parsing*/
	        if (Rx_Buffer[str_index] == 'I')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < str_index + 10; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                RelativeI = atof (strm);
	                free(strm);
	            }
	        }
	        if (Rx_Buffer[str_index] == ' ')
	        {
	            str_index++;
	        }
	        /*J parsing*/
	        if (Rx_Buffer[str_index] == 'I')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < str_index + 10; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                RelativeJ = atof (strm);
	                free(strm);
	            }
	        }
	        if (Rx_Buffer[str_index] == ' ')
	        {
	            str_index++;
	        }
	        /*K parsing*/
	        if (Rx_Buffer[str_index] == 'K')
	        {
	            temp = 0;
	            for (i = str_index + 1; i < str_index + 10; i++)
	            {
	                if (((Rx_Buffer[i] > 0x29) && (Rx_Buffer[i] < 0x40)) || (Rx_Buffer[i] == '-') || (Rx_Buffer[i] == '.') )
	                {
	                    temp++;
	                }
	                else 
	                {
	                    break;
	                }
	            }
	            if (temp != 0)
	            {
	                str_index = str_index + temp + 1;
	                strm = (char*) malloc(temp);
	                memcpy (strm, &Rx_Buffer[str_index - temp], temp);
	                RelativeK = atof (strm);
	                free(strm);
	            }
	        }
	        //F
	        ret = TRUE;
		}
		else
		{
			/*When a line is started with (, then it is a comment and processing is ignored*/
			/*Send XON to start receiving new command*/
			HAL_Delay(50);
			HAL_UART_Transmit_IT(&huart2,Tx_XON,1);
            HAL_UART_Receive_IT(&huart2, Rx_data, 1);	//activate UART receive interrupt every time
		}
    #endif
    }
    else
    {
        /*Do nothing*/
    }
   
    return ret;
}

#ifdef  DEFINED_COMMAND_PARSER
static real32 FloatExtract (uint8_t index, uint8_t* data)
{
    real32 ret;
    ret = (real32)((data[index+1] - 0x30)*100) + (real32)((data[index+2] - 0x30)*10) + (real32)(data[index+3] - 0x30) + 
                                  ((real32)(data[index+4] - 0x30))*0.1 + ((real32)(data[index+5]) - 0x30)*0.01;
    if (data[index] == '+')
    {
        return ret;
    }
    else
    {
        return -ret;
    }
    
}
#endif
