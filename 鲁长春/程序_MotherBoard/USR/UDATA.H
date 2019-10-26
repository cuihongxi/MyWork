#ifndef __UDATA_H
#define __UDATA_H
/********************************************************************************
*Author :       
*Time   :       2019-8-16 09:54:45
*Explain:       所有需要修改的参数在这里修改
*
********************************************************************************/
#define			VALVE_BAT_GREEN		8.4	// v,＞8.4V时绿灯常亮。
#define			VALVE_BAT_SHARP3	7.4	// v,＞7.4V绿灯闪亮3次；＜7.4V红灯闪亮3次。后转为每3秒闪亮一次红灯欠压指示。
#define         	VALVE_BAT_Motor         7.2     // v,电池马达关窗阀值，低于该值动作
#define         	VALVE_BAT_NoBACK        4.9    	// v,不再响应反转信号
#define			VALVE_BAT_CHECK		7.4	// v,大于此电压，间隔TIM_BAT_CHECKH检测一次，小于则间隔TIM_BAT_CHECKL检测一次
#define			TIM_BAT_CHECKH		10000	// ms,大于VALVE_BAT_CHECK电压，检测间隔
#define			TIM_BAT_CHECKL		2000	// ms,小于VALVE_BAT_CHECK电压，检测间隔
#define			VALVE_BAT_CHARGE	8.3	// v,充电大于则蓝灯常量

#define			TIM_MOTOR_Z		400	// ms,马达到达BC1,或BC2时继续向前的时间，单位ms
#define			TIM_MOTOR_F		400	// ms,马达停转时反转时间，解锁离合，单位ms
#define         	VALVE_YS_D              2     	// V，雨水传感器的感应电压阀值
#define         	TIM__YS_D              	4000    // ms，雨水传感器的滤波间隔
#define         	TIM__FL_D              	4000    // ms，FL传感器的滤波间隔
#define         	VALVE_FLSPEED           200     // 每分钟200转为FL转速阀值
#define         	MOTOR_F_SAFE            60000   // ms,正转保护
#define			TIM_SLEEP		5000	// ms,休眠倒计时时间
#define			TIM_KEY			1000	// ms，连击间隔
#define         	TIM_CHECKEYS          	2000    // ms,YS检测时间
#define         	BH_SAFE                 5000    // ms, BH方波检测
#define         	TIM_BH0                 4000    // ms, BH反向运动时间
#define         	TIM_30                  1	// 30分钟不响应
#define         	TIM_BAT                 5000    // ms ,检测BAT电压周期
#define			TIM_AM_ON		3000	// ms,AM有效时对应的LED指示点亮时间
#define			TIM_AM_OFF		500	// ms,AM无效时对应的LED指示点亮时间

#define			TIM_Y30_OFF		500	// ms,Y30无效时对应的LED指示点亮时间
#define			TIM_OPEN		5000    // ms,无YS计时开窗
#define			TIM_Y30_DELAY		1500	// ms,Y30长按3S取消功能
#define			TIM_DML			1000	// ms, 大于这个值，并小于TIM_DMH 内抬起DM，进入NRF对码
#define			TIM_DMH			3000	// ms, 小于这个值，并大于TIM_DML 内抬起DM，进入NRF对码
#define			TIM_DM_CLEARL		6000	// ms, 清除对码值下限
#define			TIM_DM_CLEARH		12000	// ms, 清除对码值上限
#define			TIM_BH_FILTER		50	// ms, BH滤波时间
#define			TIM_FL_FILTER		20	// ms, FL滤波时间
#define			TIM_LED_SHARP_ON	100	//ms，设置参数LED闪烁亮的时间
#define			TIM_LED_SHARP_OFF	100	//ms，设置参数LED闪烁灭的时间
#define			TIM_MOTORLED_SHARP_ON	100	//ms，马达运行LED闪烁亮的时间
#define			TIM_MOTORLED_SHARP_OFF	100	//ms，马达运行LED闪烁灭的时间

#define			STEP_MOTO		10	// 每按一次，马达行走的步数
#define			TIM_MOTO_HOLD		500	// ms,马达刹车时间

#endif