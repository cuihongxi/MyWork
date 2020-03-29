#ifndef __UDATA_H
#define __UDATA_H
/********************************************************************************
*Author :      
*Time   :       2019年11月11日08:13:52
*Explain:       所有需要修改的参数在这里修改
*
********************************************************************************/

#define         DM_NUM              10      	//对码次数，超过次数没对码成功，退出对码模式
#define  		REUSE_TIMES         20  		// 发送模式，发送不成功时重发的次数,40*60ms
//#define  		DELAY_TIME          2000 		// ms,长按延时

#define         VALVE_FLSPEED_0       200      	// 每分钟200转为FL转速阀值
#define         VALVE_FLSPEED_1       500      	// 每分钟500转为FL转速阀值
#define         VALVE_FLSPEED_2       1200      // 每分钟1200转为FL转速阀值

#define         TIM__FL_D           3000     	// ms，FL传感器的滤波间隔，有信号的持续时间？
#define  		TIM_FL_FILTER     	20   		// ms, FL滤波时间，检测持续时间？

#define  		TIM_YS_ADC    		2000  		// ms,YS检查周期 ，多长时间检测一次。
#define         VALVE_YS_D_0        1.2     	// V，雨天雨水传感器的感应电压阀值
#define         VALVE_YS_D_1        2     		// V，雨天雨水传感器的感应电压阀值
#define         VALVE_YS_D_2        2.8     	// V，雨天雨水传感器的感应电压阀值

#define         TIM_YS_30M         	60000      //1800000     // ms,YS持续超过20分钟，每30分钟发送一次
#define         TIM_FL            	180000      //1800000     // ms,如果FL超限，的检测间隔每30分钟发送一次

#define  		TIM_BAT_6H    		21600000    //21600000 // ms, 6H间隔
#define  		TIM_BAT_2H    		7200000     //7200000  // ms, 2H间隔
#define  		TIM_BAT_0_5H    	1800000     //1800000  // ms, 0.5H间隔
#define         TIM__YS_D          	120         // ms，雨水传感器的滤波间隔，YS—D供电时间？供电时长？
#define         TIM_CHECKEYS       	4000         // ms,YS检测时间，电池检测控制端时间？

#define  		TIM_CLEARDM    		16000 		//清除DM的延时
#define  		TIM_CLOSE    		4000  		//开关机的延时


#endif





