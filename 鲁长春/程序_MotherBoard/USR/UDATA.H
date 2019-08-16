#ifndef __UDATA_H
#define __UDATA_H
/********************************************************************************
*Author :       
*Time   :       2019-8-16 09:54:45
*Explain:       所有需要修改的参数在这里修改
*
********************************************************************************/
//#define DEBUG_UART

#define         TIM_LH_SLEEP            10      // LH闭合后，TIM_LH秒进入休眠状态
#define         TIM_LH_SAFE             90      // LH断开TIM_LH_SAFE时间，反转。单位秒

#ifdef  DEBUG_UART
#define         TIM_ANSWER              1        
#else
#define         TIM_ANSWER              30      // 不响应YS_FL信号的时间，单位分
#endif

#define         TIM_checkYSFL           1       // YSFL检测时间，单位秒
#define         TIM_WZ                  5       // WZ闭合后，时间TIM_WZ内不响应YS_FL信号，单位秒
#define         TIM_BAT                 5       // 检测BAT电压周期，单位秒
#define         TIM_MOTOSTOP0           1.5     // 马达正常停转时间，单位秒
#define         TIM_MOTOSTOP1           2       // BH方波异常，马达反转时间，单位秒
#define         TIM_MOTOSTART0          2       // 马达停止后正转时间，单位秒
#define         TIM_MOTOSTART1          6       // BH方波异常，马达停转后正转时间，单位秒
#define         TIM_WakeUp_Low          3       // 3秒唤醒检测电池电压
#define         TIM_WakeUp_High         27      // 27秒唤醒检测电池电压，不能超过27，否则溢出
#define         TIM_BACK                10      // 马达反转10秒进入重启
#define         TIM_BH0                 0.5     // BH又出现0.5秒后进入重启

#define         MOTOR_F_SAFE            50      // 正转保护，单位秒   
#define         BH_SAFE                 5       // BH方波检测，单位秒

#define         VALVE_BAT               7.2     // 电池7.2V阀值，低于该值动作
#define         VALVE_BAT_Motor         6.8     // 电池马达6.8V阀值，低于该值动作
//#define         VALVE_YS                332.0   // YS阀值电阻，K欧,根据需要修改
#define         VALVE_YS_D              2     // V，雨水传感器的感应电压阀值

#define         VALVE_FLSPEED           200     // 每分钟200转为FL转速阀值
#define         VALVE_USB               150     // USB充电口检测



#endif