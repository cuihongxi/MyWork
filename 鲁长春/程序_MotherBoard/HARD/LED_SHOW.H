#ifndef __LED_SHOW_H
#define __LED_SHOW_H
/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       共地双色LED指示灯
*
********************************************************************************/


#define         LED1_GREENGPIO          GPIOB,GPIO_Pin_5          //绿灯
#define         LED2_REDGPIO            GPIOD,GPIO_Pin_1         //红灯




void LED_GPIO_Init();   //双色LED初始化

#endif