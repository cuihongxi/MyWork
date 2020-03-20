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
#define         BatControl_GPIO         GPIOA,GPIO_Pin_6  	//控制端

#define         YS_GPIO                 GPIOB,GPIO_Pin_5                
#define         YSD_GPIO                GPIOB,GPIO_Pin_3 	//给YS传感器供电

#define			YS_RT_GPIO				GPIOD,GPIO_Pin_6 	//电位器设定
#define			FL_RT_GPIO				GPIOB,GPIO_Pin_6 	//电位器设定
#define			YS_RT_GND				GPIOC,GPIO_Pin_4	
#define			YS_RT_VCC				GPIOD,GPIO_Pin_5
#define			FL_RT_GND				GPIOD,GPIO_Pin_4	
#define			FL_RT_VCC				GPIOB,GPIO_Pin_7

typedef enum 
{
    Battery_Channel     = ADC_Channel_1,  
	YS_Channel 			= ADC_Channel_13,
	YS_RT_Channel		= ADC_Channel_8,
	FL_RT_Channel		= ADC_Channel_12,
}hardChannel;

typedef	enum{
	none,
	Rainy,
	Sunny,
}enumWeather;


//函数

void GPIO_ADC_Init();   //YS,Battery的GPIO初始化
uint16_t Get_ADC_Dat(hardChannel hard_channel);
float BatteryGetAD(u16 ad);	// 根据AD值计算电池端电压
float YSGetAD(u16 ad);		// 根据AD值计算YS端电压
void YS_Function();

void SetVal_Task(void);// 电位器设定任务
#endif
