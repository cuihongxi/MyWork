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

#define         VALVE_FLSPEED           200     // 每分钟200转为FL转速阀值
#define         TIM__FL_D              	4000    // ms，FL传感器的滤波间隔
#define			TIM_FL_FILTER		20	// ms, FL滤波时间

#define         IRQ_PERIOD      	500

#endif