/**
 *  低功耗配置 
 */

#ifndef __lowpower_h
#define __lowpower_h

#include "uhead.h"

#define		GPIO_DER_Z			GPIOB,GPIO_Pin_0 		// >z 触摸板，应该进行上升沿中断检测
#define		GPIO_AM				GPIOB,GPIO_Pin_2 		// AM 触摸板，应该进行上升沿中断检测
#define		GPIO_DER_Z			GPIOB,GPIO_Pin_0 		// <z 触摸板，应该进行上升沿中断检测
#define		GPIO_DER_Y			GPIOB,GPIO_Pin_3		// >Y触摸板，应该进行上升沿中断检测	
#define		GPIO_BH				GPIOC,GPIO_Pin_0		// BH方波检测,下降沿中断,当马达不动时应该停止中断检测,并置1
#define		GPIO_FLU			GPIOA,GPIO_Pin_6		//	FL-U风力检测,下降沿中断
#define		GPIO_38KHZ_OUT		GPIOD,GPIO_Pin_5		// 38khz发射口，定时器输出	 
#define		GPIO_AM_D			GPIOB,GPIO_Pin_6		// 触摸LED
#define		GPIO_Y30_D			GPIOD,GPIO_Pin_6		// Y30 led


#define		GPIO_HWKZ			GPIOB,GPIO_Pin_4 		// 红外控制
#define		GPIO_38KHZ_BC1		GPIOB,GPIO_Pin_7		// 红外检测返回信号，高电平有效
#define		GPIO_38KHZ_BC2		GPIOD,GPIO_Pin_4		// 红外检测返回信号，高电平有效	 
#define		GPIO_DM				GPIOD,GPIO_Pin_7		// 对码键
#define		GPIO_BAT_CON		GPIOA,GPIO_Pin_0		// 电池电量检测控制
#define		GPIO_SYSLED_G		GPIOB,GPIO_Pin_5		// 系统LED 绿色
#define		GPIO_SYSLED_R		GPIOD,GPIO_Pin_1		// 系统LED 红色
	 





	 
	 
void LowPowerSet(void);
#endif
