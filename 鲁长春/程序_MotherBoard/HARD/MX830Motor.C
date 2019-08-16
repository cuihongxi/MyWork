#include "UHEAD.H"
#include "MX830Motor.h"
//#include "LED_SHOW.H"
//#include "HARD.H"
Motor_Struct    motorStruct = {0,STOP};         // 马达状态结构体

/**************************************
*@brief
*@note 
*@param
*@retval
***************************************/

void MX830Motor_GPIOInit()
{
    GPIO_Init(MX830Motor_GPIO_FI,MX830Motor_GPIOMode);
    GPIO_Init(MX830Motor_GPIO_BI,MX830Motor_GPIOMode);
}
 
/**************************************
*@brief
*@note 
*@param
*@retval
***************************************/

void MX830Motor_StateDir(Motor_Struct* motorStruct)
{ 
    motorStruct->flag_dirChanged = 0;
    switch(motorStruct->dir)
    {
        case FORWARD    :     
        debug("dir = FORWARD\r\n");           
          GPIO_SetBits(MX830Motor_GPIO_FI);
		  GPIO_ResetBits(MX830Motor_GPIO_BI);
//		  GPIO_SetBits(LED2_REDGPIO);
//		  GPIO_ResetBits(LED1_GREENGPIO);
          break;
        case BACK       :        
        debug("dir = BACK\r\n"); 
                GPIO_ResetBits(MX830Motor_GPIO_FI);
                GPIO_SetBits(MX830Motor_GPIO_BI);
//                GPIO_ResetBits(LED2_REDGPIO);
//                GPIO_SetBits(LED1_GREENGPIO);
//                count_MOTOR_F = 0;                      // 正转计时清零
          break;
        case STOP       :  
          {       
        		debug("dir = STOP\r\n");
                GPIO_ResetBits(MX830Motor_GPIO_FI);
                GPIO_ResetBits(MX830Motor_GPIO_BI);
//                GPIO_ResetBits(LED2_REDGPIO);
//                GPIO_ResetBits(LED1_GREENGPIO);
//                count_MOTOR_F = 0;	// 正转计时清零
//                count_BH = 0;       // 计数清零
                
          }          
          break;
        case HOLD       :  
		  GPIO_SetBits(MX830Motor_GPIO_FI); 
		  GPIO_SetBits(MX830Motor_GPIO_BI);
          break;
    }
}
           
