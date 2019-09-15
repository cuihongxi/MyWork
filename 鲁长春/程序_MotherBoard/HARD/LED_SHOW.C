#include "UHEAD.H"
#include "LED_SHOW.H"



//Ë«É«LED³õÊ¼»¯
void LED_GPIO_Init()
{
    GPIO_Init(LED_GREEN,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(LED_RED,GPIO_Mode_Out_PP_Low_Slow);
}

//
//void LED_GREEN_Open(u8 bol)
//{
//	if(bol) 
//	{
//		GPIO_SET(LED_GREEN);
//		GPIO_RESET(LED_RED);
//	}else
//	{
//		GPIO_RESET(LED_GREEN);
//	}
//}
//
//void LED_RED_Open(u8 bol)
//{
//	if(bol) 
//	{
//		GPIO_SET(LED_RED);
//		GPIO_RESET(LED_GREEN);
//	}else
//	{
//		GPIO_RESET(LED_RED);
//	}
//}

void LEN_RED_Open()
{
	GPIO_SET(LED_RED);
//	GPIO_RESET(LED_GREEN);
}

void LEN_RED_Close()
{
	GPIO_RESET(LED_RED);
}

void LEN_GREEN_Open()
{
	GPIO_SET(LED_GREEN);
//	GPIO_RESET(LED_RED);
}

void LEN_GREEN_Close()
{
	GPIO_RESET(LED_GREEN);
}



