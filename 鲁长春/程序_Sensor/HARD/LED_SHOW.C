#include "UHEAD.H"
#include "LED_SHOW.H"

void LEN_RED_Open()
{
	GPIO_RESET(LED_RED);
}

void LEN_RED_Close()
{
	GPIO_SET(LED_RED);
}	
void LEN_GREEN_Open()
{
	GPIO_RESET(LED_GREEN);
}

void LEN_GREEN_Close()
{
	GPIO_SET(LED_GREEN);
}
//Ë«É«LED³õÊ¼»¯
void LED_GPIO_Init()
{
    GPIO_Init(LED_RED,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(LED_GREEN,GPIO_Mode_Out_OD_HiZ_Slow);
	LEN_RED_Close();
	LEN_GREEN_Close();
	delay_ms(200);
	LEN_GREEN_Open();
	delay_ms(200);
	LEN_GREEN_Close();
	delay_ms(200);
	LEN_GREEN_Open();
	delay_ms(200);
	LEN_GREEN_Close();
//	delay_ms(200);
//	LEN_GREEN_Open();
//	delay_ms(200);
//	LEN_GREEN_Close();
}
