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

#define         Battery_GPIO            GPIOA,GPIO_Pin_5                
#define         YS_GPIO                 GPIOC,GPIO_Pin_4                
#define         YSD_GPIO                GPIOA,GPIO_Pin_4 	//给YS传感器供电

#define         CHARGE_PRO_PIN			GPIOD,GPIO_Pin_0   	//充电保护 

#define			GPIO_TEMP				GPIOC,GPIO_Pin_1			// 过热保护，AD > 2.85v 停止马达


typedef enum 
{
    Battery_Channel     = ADC_Channel_1,  
	YS_Channel 			= ADC_Channel_4,
}hardChannel;

extern  u16     VALVE_BAT_ADCval        ;                       //电池7.2V阀值，低于该值动作        
extern  u16     VALVE_YS_ADCval         ;                       //雨水传感器的转换阀值，小于该值动作

//函数

void GPIO_ADC_Init();   //YS,Battery的GPIO初始化
uint16_t Get_ADC_Dat(hardChannel hard_channel);

#endif
