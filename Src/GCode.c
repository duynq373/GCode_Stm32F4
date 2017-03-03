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
static boolean _IsNewCommandReceived(void);
static real32 FloatExtract (uint8_t index, uint8_t* data);
static void _CalcNextCoord (void);
static void _IsStep(Coord* _current, Coord* _cal);
static void SendPulse (void);
static uint8_t PulseWidth (uint32_t* counter, boolean* initflag);

/* Private variables ---------------------------------------------------------*/

real32 steplength_x = (real32)0.01;       /*    steplength for X axis*/
real32 steplength_y = (real32)0.1;        /*    steplength for Y axis*/
real32 steplength_z = (real32)0.1;        /*    steplength for Z axis*/
boolean isPulseRunning = FALSE;
//uint8_t PulseWidthMap[START_STOP_PERIOD] = {
//    50	,    //1
//    50	,    //2
//    50	,    //3
//    50	,    //4
//    50	,    //5
//    50	,    //6
//    50	,    //7
//    50	,    //8
//    50	,    //9
//    50	,    //10
//    45	,    //11
//    45	,    //12
//    45	,    //13
//    45	,    //14
//    45	,    //15
//    45	,    //16
//    45	,    //17
//    45	,    //18
//    45	,    //19
//    45	,    //20
//    35	,    //21
//    35	,    //22
//    35	,    //23
//    35	,    //24
//    35	,    //25
//    35	,    //26
//    35	,    //27
//    35	,    //28
//    35	,    //29
//    35	,    //30
//    25	,    //31
//    25	,    //32
//    25	,    //33
//    25	,    //34
//    25	,    //35
//    25	,    //36
//    25	,    //37
//    25	,    //38
//    25	,    //39
//    25	,    //40
//    15	,    //41
//    15	,    //42
//    15	,    //43
//    15	,    //44
//    15	,    //45
//    15	,    //46
//    15	,    //47
//    15	,    //48
//    15	,    //49
//    15	     //50
//};

uint8_t PulseWidthMap[START_STOP_PERIOD] = {
    0	,	//1
    0	,	//2
    0	,	//3
    0	,	//4
    0	,	//5
    0	,	//6
    0	,	//7
    0	,	//8
    0	,	//9
    0	,	//10
    0	,	//11
    0	,	//12
    0	,	//13
    0	,	//14
    0	,	//15
    0	,	//16
    0	,	//17
    0	,	//18
    0	,	//19
    0	,	//20
    0	,	//21
    0	,	//22
    0	,	//23
    0	,	//24
    0	,	//25
    0	,	//26
    0	,	//27
    0	,	//28
    0	,	//29
    0	,	//30
    0	,	//31
    0	,	//32
    0	,	//33
    0	,	//34
    0	,	//35
    0	,	//36
    0	,	//37
    0	,	//38
    0	,	//39
    0	,	//40
    0	,	//41
    0	,	//42
    0	,	//43
    0	,	//44
    0	,	//45
    0	,	//46
    0	,	//47
    0	,	//48
    0	,	//49
    0		//50
};


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
    uint8_t trm_bf = 1;
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
            // send feedback to PC
            HAL_UART_Transmit(&huart2,&trm_bf,1,500);
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
    InitFlag = TRUE;
    _CalcNextCoord();
//    while (_CalcNextCoord())
//    {
//        _IsStep(&Current, &Cal);
//        SendPulse();
//    }
}

/*
    Calculate the next Coordinate for movement required command G00, G01, G02, G03, etc
    @Input:
        + GCode type: linear or curve or others
        + Target coordinate value
            
*/
static void _CalcNextCoord (void)
{
//    boolean ret = FALSE;
    // STUB
    //GCode_Command = G_02;
    // END STUB
    
    switch (GCode_Command)
    {
        case G_00:
            //Code here
            break;
        
        case G_01:
            //Code here
//            if (InitFlag)
//            {
            _LinearCal_Init();
//                InitFlag = FALSE;
//            }
//            ret  = _LinearCal(&LinearStructMax, &LinearStruct_1, &LinearStruct_2);
            while (_LinearCal(&LinearStructMax, &LinearStruct_1, &LinearStruct_2))
            {
                _IsStep(&Current, &Cal);
                SendPulse();
            }
            break;
        
        case G_02:
        case G_03:
            //Code here
//            if (InitFlag)
//            {
            _Curve_Init();
//                InitFlag = FALSE;
//            }
//            ret  = _CurveCal(&Axis_1, &Axis_2, &Cur_Params); 
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
//    return ret;
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
    uint8_t delay_time = DEFAULT_WIDTH;
    if (GCode_Command == G_01)
    {
        delay_time = PulseWidth(&Linear_Counter, &InitFlag);
    }
    else if ((GCode_Command == G_02) || (GCode_Command == G_03))
    {
        delay_time = PulseWidth(&Cur_Params.RunTime, &InitFlag);
    }
    else
    {
        delay_time = DEFAULT_WIDTH;  /*Code should never goes here*/
    }
    
    if (isPulseRunning)
    {
        uint8_t index = 0; 
        while (isPulseRunning)
        {
            /*Donothing and wait for the timer to run the current pulse*/
        }
        /*Ensure the OFF period of output PULSE*/
        for (index = 0; index <= 8; index++)        // 9 * 0.6 us = 5.4 us for OFF period
        {
            PULSES_OFF;
        }
    }
    
    if (stepFlag_x == TRUE)
    {
        PULSE_X_ON;
        isPulseRunning = TRUE;
//        PULSE_X_ON;
//        PULSE_X_ON;
    }
    
    if (stepFlag_y == TRUE)
    {
        PULSE_Y_ON;
        isPulseRunning = TRUE;
//        PULSE_Y_ON;
//        PULSE_Y_ON;
    }
    if (stepFlag_z == TRUE)
    {
        PULSE_Z_ON;
        isPulseRunning = TRUE;
//        PULSE_Z_ON;
//        PULSE_Z_ON;
    }
    if (isPulseRunning)
    {
        if (delay_time != 0)
        {
            htim3.Init.Period = (uint16_t)delay_time;
            HAL_TIM_Base_Start_IT(&htim3);
        }
        else
        {
            isPulseRunning = FALSE;;
        }
        
    }
       
    // Reinit the step flags
    stepFlag_x = FALSE;
    stepFlag_y = FALSE;
    stepFlag_z = FALSE;
}


static boolean _IsNewCommandReceived(void)
{
    boolean ret = FALSE;
    uint8_t rcv_bf[UART_BUFFER_LENGTH];
    uint32_t temp = 0;
    //real32 temp_f = 0;
    if (HAL_UART_Receive(&huart2,rcv_bf,UART_BUFFER_LENGTH,500) == HAL_OK)
    {
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
    }
    else
    {
        /*Do nothing*/
    }
    
    return ret;
}

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
static uint8_t PulseWidth (uint32_t* counter, boolean* initflag)
{
    uint8_t pulsewidth;
    static uint32_t start_period;
    
    if (*initflag)
    {
        start_period = 0;    
        *initflag = FALSE;
    }
    
    pulsewidth = DEFAULT_WIDTH;    /*can be changed to a variable if support change speed*/
    
    if (start_period < START_STOP_PERIOD)   /*RISING EDGE*/
    {
        pulsewidth = PulseWidthMap[start_period];
        start_period++;
    }
    if (*counter < START_STOP_PERIOD)       /*FALLING EDGE*/
    {
        pulsewidth = PulseWidthMap[START_STOP_PERIOD - *counter];
    }
    return pulsewidth;
}
