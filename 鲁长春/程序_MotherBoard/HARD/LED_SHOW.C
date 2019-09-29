#include "UHEAD.H"
#include "LED_SHOW.H"

//Ë«É«LED³õÊ¼»¯
void LED_GPIO_Init()
{
    GPIO_Init(LED_GREEN,GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED_RED,GPIO_Mode_Out_PP_High_Slow);
}

void LEN_RED_Open()
{
	GPIO_RESET(LED_RED);
	GPIO_SET(LED_GREEN);
}

void LEN_RED_Close()
{
	GPIO_SET(LED_RED);
}

void LEN_GREEN_Open()
{
	GPIO_RESET(LED_GREEN);
	GPIO_SET(LED_RED);
}

void LEN_GREEN_Close()
{
	GPIO_SET(LED_GREEN);
}



