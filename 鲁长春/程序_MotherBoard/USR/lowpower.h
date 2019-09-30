/**
 *  低功耗配置 
 */

#ifndef __lowpower_h
#define __lowpower_h

#include "uhead.h"



#define		GPIO_BH2		GPIOD,GPIO_Pin_6	
#define		GPIO_BH			GPIOB,GPIO_Pin_6		// BH方波检测,下降沿中断,当马达不动时应该停止中断检测,并置1
#define		GPIO_FLU		GPIOA,GPIO_Pin_6		// FL-U风力检测,下降沿中断
#define		GPIO_38KHZ_OUT		GPIOD,GPIO_Pin_5		// 38khz发射口，定时器输出	 


#define		GPIO_38KHZ_BC1		GPIOB,GPIO_Pin_7		// 红外检测返回信号，低电平有效
#define		GPIO_38KHZ_BC2		GPIOD,GPIO_Pin_4		// 红外检测返回信号，低电平有效	 

#define		GPIO_BAT_CON		GPIOA,GPIO_Pin_0		// 电池电量检测控制

	 




	 
	 
void LowPowerSet(void);






#endif


