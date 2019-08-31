#ifndef __UDATA_H
#define __UDATA_H
/********************************************************************************
*Author :       
*Time   :       2019-8-16 09:54:45
*Explain:       所有需要修改的参数在这里修改
*
********************************************************************************/
#define			VALVE_BAT_GREEN			8.4		// ＞8.4V时绿灯常亮。
#define			VALVE_BAT_SHARP3		7.4		// ＞7.4V绿灯闪亮3次；＜7.4V红灯闪亮3次。后转为每3秒闪亮一次红灯欠压指示。
#define         VALVE_BAT_Motor         7.2     // 电池马达关窗阀值，低于该值动作
#define         VALVE_BAT_NoBACK        4.9    	// 不再响应反转信号

#define			TIM_MOTOR_Z				1000	// 马达到达BC1,或BC2时继续向前的时间，单位ms
#define			TIM_MOTOR_F				1500	// 马达到达BC1,或BC2时反向的时间，单位ms
#define         VALVE_YS_D              2     	// V，雨水传感器的感应电压阀值
#define         TIM__YS_D              	4     	// s，雨水传感器的滤波间隔
#define         TIM__FL_D              	4     	// s，FL传感器的滤波间隔
#define         VALVE_FLSPEED           200     // 每分钟200转为FL转速阀值
#define         MOTOR_F_SAFE            60      // 正转保护，单位秒
#define			TIM_SLEEP				5000	// ms,休眠倒计时时间
#define			TIM_KEY					600		// ms，连击间隔
#define         TIM_CHECKEYS          	2000    // ms,YS检测时间
#define         BH_SAFE                 5000    // ms, BH方波检测
#define         TIM_BH0                 4000    // ms, BH反向运动时间
#define         TIM_30                  60000*30// 30分钟不响应
#define         TIM_BAT                 5000    // ms ,检测BAT电压周期



#define         TIM_MOTOSTOP0           1.5     // 马达正常停转时间，单位秒
#define         TIM_MOTOSTOP1           2       // BH方波异常，马达反转时间，单位秒
#define         TIM_MOTOSTART0          2       // 马达停止后正转时间，单位秒
#define         TIM_MOTOSTART1          6       // BH方波异常，马达停转后正转时间，单位秒
#define         TIM_WakeUp_Low          6*3600  // 6小时唤醒检测电池电压
#define         TIM_WakeUp_High         27      // 27秒唤醒检测电池电压，不能超过27，否则溢出
#define         TIM_BACK                10      // 马达反转10秒进入重启


   





#endif