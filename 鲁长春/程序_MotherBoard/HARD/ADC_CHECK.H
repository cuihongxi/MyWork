/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       
*
********************************************************************************/



#ifndef __ADC_CHECK_H
#define __ADC_CHECK_H

#include "UHEAD.H"
#include "CUI_RTOS.H"
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

extern u8 flag_YS ;					// YS信号超警戒,启动计时器，超过4S，触发flag_YS_SHUT = 1
extern u8 flag_YS_SHUT ;			
extern u16 counter_YS ;				//YS信号产生后计时



//函数

void GPIO_ADC_Init();   //YS,Battery的GPIO初始化
uint16_t Get_ADC_Dat(hardChannel hard_channel);
float BatteryGetAD(u16 ad);	// 根据AD值计算电池端电压
float YSGetAD(u16 ad);		// 根据AD值计算YS端电压
void YS_Control();			// YS控制

#endif
