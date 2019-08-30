#include "UHEAD.H"
#include "LED_SHOW.H"



//双色LED初始化
void LED_GPIO_Init()
{
    GPIO_Init(LED_GREEN,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(LED_RED,GPIO_Mode_Out_PP_Low_Slow);
}


void LED_GREEN_Open(u8 bol)
{
	if(bol) 
	{
		GPIO_SET(LED_GREEN);
		GPIO_RESET(LED_RED);
	}else
	{
		GPIO_RESET(LED_GREEN);
	}
}

void LED_RED_Open(u8 bol)
{
	if(bol) 
	{
		GPIO_SET(LED_RED);
		GPIO_RESET(LED_GREEN);
	}else
	{
		GPIO_RESET(LED_RED);
	}
}

void LEN_RED_Open()
{
	GPIO_SET(LED_RED);
	GPIO_RESET(LED_GREEN);
}

void LEN_RED_Close()
{
	GPIO_RESET(LED_RED);
}

void LEN_GREEN_Open()
{
	GPIO_SET(LED_GREEN);
	GPIO_RESET(LED_RED);
}

void LEN_GREEN_Close()
{
	GPIO_RESET(LED_GREEN);
}

//os下LED闪烁设定
void OS_LED_Sharp(TaskStr* task,LEDType led,u16 time,u8 count)
{
	if(led == GREEN)
		while(count--)
		{
			OS_AddFunction(task,LEN_GREEN_Open,time);	
			OS_AddFunction(task,LEN_GREEN_Close,time);	
		}
	if(led == RED)
		while(count--)
		{
			OS_AddFunction(task,LEN_RED_Open,time);	
			OS_AddFunction(task,LEN_RED_Close,time);	
		}
		
	
}


