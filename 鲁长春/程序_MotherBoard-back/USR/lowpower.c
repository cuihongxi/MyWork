#include "lowpower.h"
#include "24l01.h"
#include "ADC_CHECK.H"
#include "MX830Motor.h"

void LowPowerGPIO()
{
	GPIO_RESET(GPIO_HWKZ);					// 红外控制
	GPIO_RESET(GPIO_38KHZ_BC1);				// 红外检测返回信号
	GPIO_RESET(GPIO_38KHZ_BC2);				// 红外检测返回信号
	GPIO_RESET(MX830Motor_GPIO_FI);			// 马达 IO
	GPIO_RESET(MX830Motor_GPIO_BI);			// 马达 IO
	GPIO_RESET(GPIO_DM);					// 对码
	GPIO_RESET(YSD_GPIO);					// 雨水检测 供电
	GPIO_RESET(GPIO_BAT_CON);				// 电池电量检测控制
	GPIO_RESET(GPIO_SYSLED_G);				// 系统LED
	GPIO_RESET(GPIO_SYSLED_R);				// 系统LED
	
	//NRF低功耗端口配置
	
}


void LowPowerSet(void)
{
	LowPowerGPIO();
	
}