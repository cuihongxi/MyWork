/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       
*
********************************************************************************/



#ifndef __ADC_CHECK_H
#define __ADC_CHECK_H

#include "UHEAD.H"
#include "stm8l15x_adc.h"

#define         Battery_GPIO            GPIOB,GPIO_Pin_0  
#define         BatControl_GPIO         GPIOC,GPIO_Pin_0  //控制端


typedef enum 
{
    	Battery_Channel     = ADC_Channel_18,  
}hardChannel;



//函数

void GPIO_ADC_Init();   //YS,Battery的GPIO初始化
uint16_t Get_ADC_Dat(hardChannel hard_channel);
float BatteryGetAD(u16 ad);	// 根据AD值计算电池端电压


#endif
