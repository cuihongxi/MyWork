#include "ADC_CHECK.H"
#include "lowpower.h"
#include "keyboard.h"

JugeCStr 	jugeYS_No 	= {0};			// 无雨水，AM打开时计时开窗
JugeCStr 	jugeYS 		= {0};			// YS信号超警戒,启动计时器，超过4S，触发	
float 		YSdat 		= 0;			// ys-u
extern		TaskStr* 	taskYS		;	// YS测量任务
				
JugeCStr 	YS_30 		= {0};			// YS供电标志位，当按下按键30分钟不响应YS信号
u32			ys_timer30 	= 0;			//YS不响应计时
extern TaskLinkStr* tasklink;			// 任务列表

//根据AD值计算电池端电压
float BatteryGetAD(u16 ad)
{
	return (0.0024175824175*ad);
}

//根据AD值计算YS端电压
float YSGetAD(u16 ad)
{
	return (3.3*ad)/65535;
}



//YS,FL,Battery的GPIO初始化
void GPIO_ADC_Init()
{
    GPIO_Init(Battery_GPIO,GPIO_Mode_In_FL_No_IT);
    GPIO_Init(YS_GPIO,GPIO_Mode_In_FL_No_IT);  				
	GPIO_Init(CHARGE_PRO_PIN,GPIO_Mode_In_FL_No_IT); 		//充电保护，高电平需要保护
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
}

//返回相应通道ADC值
uint16_t Get_ADC_Dat(hardChannel hard_channel)
{
    uint16_t dat= 0;
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,ENABLE);                              //打开ADC时钟                                                                            //等待时钟稳定
    ADC_Init(ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_1);      //单次转换，12位，1分频
    ADC_Cmd(ADC1,ENABLE);                                                               //使能
    ADC_ChannelCmd(ADC1,(ADC_Channel_TypeDef)hard_channel,ENABLE);                      //选择通道号 
    
    ADC_SoftwareStartConv (ADC1);                                                       //开启软件转换
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);                               //等待ADC转换完成         
    dat = ADC_GetConversionValue (ADC1);                                                //获取转换值
    ADC_ChannelCmd(ADC1,(ADC_Channel_TypeDef)hard_channel,DISABLE);                                        
    ADC_Cmd(ADC1,DISABLE); 
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);
    ADC_ClearFlag (ADC1,ADC_FLAG_EOC);                                                //清除相关标识
    
    return dat;
    
}

void YS_Check_Start()
{

}

void YS_Check_Stop()
{

}


//YS检测任务
void YS_Function()
{
	GPIO_SET(YSD_GPIO);
	YSdat = YSGetAD(Get_ADC_Dat(YS_Channel));
	GPIO_RESET(YSD_GPIO);
	if(YSdat > VALVE_YS_D )	//超过报警阀值
	{
		if(jugeYS.switchon == 0 && GPIO_READ(GPIO_38KHZ_BC1) != RESET)jugeYS.start = 1;	//开着窗
		if(key_AM.val == on && GPIO_READ(GPIO_38KHZ_BC1) == RESET)						//关着窗
		{
			jugeYS_No.counter = 0;	//清空计数
			jugeYS_No.start = 0;	
		}
			
	}
	else 
	{
		jugeYS.start = 0;
		jugeYS.counter = 0;
		if(key_AM.val == on && GPIO_READ(GPIO_38KHZ_BC1) == RESET && jugeYS_No.switchon == 0)//关着窗
			jugeYS_No.start = 1;	//开始无YS计时。超过阀值，自动开窗
	}
	debug("YSdat = %f\r\n",YSdat);
}
//YS控制
void YS_Control()
{
	static u8 flag_0 = 0;
	static u8 flag_1	= 0;				
	if(taskYS->state == Wait || taskYS->state == Stop)
	{
		if(((GPIO_READ(GPIO_38KHZ_BC1) == RESET && key_AM.val == off)|| YS_30.start) && flag_0 == 0) 	//不检测YS
		{
			flag_0 = 1;
			flag_1 = 0;
			GPIO_RESET(YSD_GPIO);									//关闭YS电源		
			OS_AddFunction(taskYS,OS_DeleteTask,0);
			OS_AddTask(tasklink,taskYS);							// 删除检测任务			
		}
		if(GPIO_READ(GPIO_38KHZ_BC1) != RESET && YS_30.start == 0 && flag_0)
		{
			flag_0 = 0;
		}
		
		if((GPIO_READ(GPIO_38KHZ_BC1) != RESET ||(GPIO_READ(GPIO_38KHZ_BC1) == RESET && key_AM.val == on))\
			&& YS_30.start == 0 && flag_1 == 0 )	//开着窗或者关着窗并且AM打开并且没有30分钟限制
		{
			flag_1= 1;
			OS_AddFunction(taskYS,OS_DeleteTask,0);
			OS_AddFunction(taskYS,YS_Function,TIM_CHECKEYS);
			OS_AddTask(tasklink,taskYS);							// 添加检测任务
		}		
	}

}

