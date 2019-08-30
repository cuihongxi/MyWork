#include "lowpower.h"
#include "24l01.h"
#include "ADC_CHECK.H"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "keyboard.h"

void LowPowerGPIO()
{
	GPIO_RESET(GPIO_38KHZ_BC1);				// �����ⷵ���ź�
	GPIO_RESET(GPIO_38KHZ_BC2);				// �����ⷵ���ź�
	GPIO_RESET(MX830Motor_GPIO_FI);			// ��� IO
	GPIO_RESET(MX830Motor_GPIO_BI);			// ��� IO
	GPIO_RESET(GPIO_DM);					// ����
	GPIO_RESET(YSD_GPIO);					// ��ˮ��� ����
	GPIO_RESET(GPIO_BAT_CON);				// ��ص���������
	GPIO_RESET(LED_GREEN);					// ϵͳLED
	GPIO_RESET(LED_RED);					// ϵͳLED
	
	//NRF�͹��Ķ˿�����
	
}


void LowPowerSet(void)
{
	LowPowerGPIO();
	
}