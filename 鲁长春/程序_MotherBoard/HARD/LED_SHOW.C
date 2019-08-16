#include "UHEAD.H"
#include "LED_SHOW.H"

//Ë«É«LED³õÊ¼»¯
void LED_GPIO_Init()
{
    GPIO_Init(LED1_GREENGPIO,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(LED2_REDGPIO,GPIO_Mode_Out_PP_Low_Slow);
}
