#ifndef __UDATA_H
#define __UDATA_H
/********************************************************************************
*Author :      
*Time   :       2019年11月11日08:13:52
*Explain:       所有需要修改的参数在这里修改
*
********************************************************************************/

#define         DM_NUM              20     		//对码次数，超过次数没对码成功，退出对码模式
#define			REUSE_TIMES			40			// 发送模式，发送不成功时重发的次数,40*60ms
#define			DELAY_TIME			2000		// ms,长按延时

#define         VALVE_FLSPEED       200     	// 每分钟200转为FL转速阀值
#define         TIM__FL_D           4000    	// ms，FL传感器的滤波间隔
#define			TIM_FL_FILTER		20			// ms, FL滤波时间

#define			TIM_YS_ADC			2000		// ms,YS检查周期 
#define         VALVE_YS_D          2     		// V，雨天雨水传感器的感应电压阀值


#define         TIM__YS_D           3000    	// ms，雨水传感器的滤波间隔
#define         TIM_YS_30M        	1800000     // ms,YS持续超过20分钟，每30分钟发送一次

#define			TIM_BAT_6H			10000//21600000	// ms, 6H间隔
#define			TIM_BAT_2H			8000//7200000		// ms, 2H间隔
#define			TIM_BAT_0_5H		6000//1800000		// ms, 0.5H间隔
#define         TIM__YS_D              	3000    // ms，雨水传感器的滤波间隔
#define         TIM_CHECKEYS          	2000    // ms,YS检测时间

#endif





