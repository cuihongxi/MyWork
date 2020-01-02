#include "ADC_CHECK.H"
	
//根据AD值计算电池端电压
float BatteryGetAD(u16 ad)
{
	return (0.0012*ad);
}
//根据AD值计算YS端电压
float YSGetAD(u16 ad)
{
	return (0.0009*ad);//(0.0008058608058*ad);
}

//Battery的GPIO初始化
void GPIO_ADC_Init()
{
    GPIO_Init(Battery_GPIO,GPIO_Mode_In_FL_No_IT);
	GPIO_Init(BatControl_GPIO,GPIO_Mode_Out_OD_HiZ_Fast);
    GPIO_Init(YS_GPIO,GPIO_Mode_In_FL_No_IT);  				
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_Low_Slow);

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

