#include "ADC_CHECK.H"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"

void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes);//通过NRF向主板发送命令函数
extern u8	flag_nrf;
extern	u16 	YS_CGdat ;
extern	u32 	systime;
extern	Nrf24l01_PTXStr 	ptx ;
extern	u8		CGDAT[5];
u32 	value_FLSPEED = VALVE_FLSPEED_0;
extern	u32 	fl_speed_width;
float value_YS_D = VALVE_YS_D_0;

//根据AD值计算电池端电压
float BatteryGetAD(u16 ad)
{
	return (0.0012*ad);
}
//根据AD值计算YS端电压
float YSGetAD(u16 ad)
{
	return (0.0008*ad);//(0.0008058608058*ad);
}

//Battery的GPIO初始化
void GPIO_ADC_Init()
{
    GPIO_Init(Battery_GPIO,GPIO_Mode_In_FL_No_IT);
	GPIO_Init(BatControl_GPIO,GPIO_Mode_Out_OD_HiZ_Fast);
    GPIO_Init(YS_GPIO,GPIO_Mode_In_FL_No_IT);  				
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_Low_Slow);

	GPIO_Init(YS_RT_GPIO,GPIO_Mode_In_FL_No_IT);  				
	GPIO_Init(YS_RT_GND,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(YS_RT_VCC,GPIO_Mode_Out_PP_Low_Slow);
	
	GPIO_Init(FL_RT_GPIO,GPIO_Mode_In_FL_No_IT);  				
	GPIO_Init(FL_RT_GND,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(FL_RT_VCC,GPIO_Mode_Out_PP_Low_Slow);
	
	
	
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

//YS检测任务
void YS_Function()
{
  	static enumWeather	weather = none;
	static u32 time = 0;
	static u32 time30 = 0;	// 30分钟计时
	static u8 flag_sw = 0;	
	float YSdat = 0;
	
  float FL_Set = 0;
  float YS_Set = 0;
  

	
	if(flag_nrf == 0)
	{
	    GPIO_SET(YS_RT_VCC);
		  YS_Set = YSGetAD(Get_ADC_Dat(YS_RT_Channel));
		  if(YS_Set <0.5)
			value_YS_D = VALVE_YS_D_0;
		  else  if(YS_Set <2.5)
			value_YS_D = VALVE_YS_D_1;
		  else value_YS_D = VALVE_YS_D_2;
		  GPIO_RESET(YS_RT_VCC);
		  
		  GPIO_SET(FL_RT_VCC);
		  FL_Set = YSGetAD(Get_ADC_Dat(FL_RT_Channel));
		  if(FL_Set < 0.5)
			value_FLSPEED = VALVE_FLSPEED_0;
		  else  if(FL_Set < 2.5)
			value_FLSPEED = VALVE_FLSPEED_1;
		  else value_FLSPEED = VALVE_FLSPEED_2;
		  GPIO_RESET(FL_RT_VCC);
		  fl_speed_width 	= (60000/value_FLSPEED);
		  
		  
			GPIO_SET(YSD_GPIO);
			YS_CGdat = Get_ADC_Dat(YS_Channel);
			YSdat = YSGetAD(YS_CGdat);
			GPIO_RESET(YSD_GPIO);
			debug("YS_CGdat = %d\r\n",YS_CGdat);
		if(weather == none)
		{
		  
			time = systime;
			if(YSdat >= value_YS_D )	//雨天
			{
				weather = Rainy;
				debug("雨天\r\n");
			}
			else{						//晴天
				
			  weather = Sunny;	
			  debug("晴天\r\n");
			}
				
		}else
		{
			if(YSdat >= value_YS_D )	//雨天
			{	
				if(weather == Sunny)
				{
					time = systime;
					weather = Rainy;
					flag_sw = 0;
					debug("雨天\r\n");
				}else
				{
					if((systime - time) >= TIM__YS_D && flag_sw == 0) 					// 滤波4S
					{
						flag_sw = 1;
						*(u16*)CGDAT = YS_CGdat;
						time30 = 0;
						debug("发送YS信息\r\n");
						NRF_SendCMD(&ptx,CGDAT,CMD_CG_YS,CMD_CG_YS);	// 发送YS信息
					}
					else
					{
						time30 += TIM_CHECKEYS;
						if(time30 > TIM_YS_30M)			// 持续超过30分钟
						{
							time30 = 0;
							debug("雨天 YS_CGdat = %d\r\n",YS_CGdat);
							*(u16*)CGDAT = YS_CGdat;
							NRF_SendCMD(&ptx,CGDAT,CMD_CG_YS,CMD_CG_YS);	// 发送YS信息				  	
						}					
					}

				}
			}else						// 晴天
			{
				if(weather == Rainy )
				{
					time = systime;
					weather = Sunny;
					flag_sw = 0;
					debug("晴天\r\n");
				}else
				{
					if((systime - time) >= TIM__YS_D && flag_sw == 0) 					// 滤波4S
					{
						flag_sw = 1;
						time30 = 0;
						*(u16*)CGDAT = YS_CGdat;
						NRF_SendCMD(&ptx,CGDAT,CMD_CG_YS,CMD_CG_YS);	// 发送YS信息
						debug("发送YS信息\r\n");
					}
		
				}			
			}	
	}

	}
}


// 电位器设定任务
void SetVal_Task(void)
{

  
}