/********************************************************************************
*Author : 
*Time   :  2019年8月20日07:40:41
*Explain: PWM TIM3 CH1
*
********************************************************************************/

#ifndef __PWM_H
#define __PWM_H

#include "UHEAD.H"
#include "stm8l15x_tim3.h"

           
#define	PWM_PIN						GPIOB,GPIO_Pin_1        	

#define	PWM_TIME					3								/*设置定时器*/
#define	_CHANNEL					1									/*通道号*/

#define	CRYFRE						16						//定义晶振频率 ,MHZ
#define PWM_PRESCALER				128							//分频系数					
#define PWM_PERIOD        			1000 							//PWM的周期，单位us 								

#define PWM_ON               		1     	
#define PWM_OFF              		0   	



//以下隐藏宏定义

#define	PWM_BASE_INIT(time,prescaler) _PWM_BASE_INIT(time,prescaler)

#define	_PWM_BASE_INIT(_TIME,PRESCALER) 	\
do{\
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM##_TIME,ENABLE);\
	TIM##_TIME##_TimeBaseInit(TIM##_TIME##_Prescaler_##PRESCALER, TIM##_TIME##_CounterMode_Up, (PWM_PERIOD*CRYFRE)/PWM_PRESCALER);\
	TIM##_TIME##_OC1Init(TIM##_TIME##_OCMode_PWM2, TIM##_TIME##_OutputState_Enable, 0, TIM##_TIME##_OCPolarity_High, TIM##_TIME##_OCIdleState_Reset);/*先空闲后运行的模式、输出使能、占空比初始化为0、运行时为高、死区时为低*/\
	TIM##_TIME##_Cmd(ENABLE);\
}while(0)

#define	PWMOUTPUT(time,state)		_PWMOUTPUT(time,state)
#define	_PWMOUTPUT(time,state)	    TIM##time##_CtrlPWMOutputs(state)

#define	PWM_SETCMP(val)						_PWM_SETCMP(PWM_TIME,_CHANNEL,val)
#define	_PWM_SETCMP(_TIME,_CHANNEL,val)		__PWM_SETCMP(_TIME,_CHANNEL,val)	/*设置定时器比较值*/
#define	__PWM_SETCMP(_TIME,_CHANNEL,val)	TIM##_TIME##_SetCompare##_CHANNEL(val)


void PWM_Init(void);
void PWM_Status(u8 status);
void PWM_SetDutyCycle(u8 dutyCycle);

#endif

