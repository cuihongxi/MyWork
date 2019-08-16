/*
*       显示屏程序
*       需要将显示驱动移植过来
*/

#ifndef         __COG12864_H
#define         __COG12864_H

#include "UHEAD.H"

#define         PIAN    4
#define         DELAY_TIMES     //COG内部延时，这里不延时

//引脚定义
#define         COG12864_CS0_PIN         GPIOD,GPIO_Pin_0        //使能信号,低电平有效
#define         COG12864_RST_PIN         GPIOD,GPIO_Pin_1        //复位
#define         COG12864_RS_PIN          GPIOD,GPIO_Pin_2        //数据命令选择,高数据,低命令
#define         COG12864_CLK_PIN         GPIOD,GPIO_Pin_3       //时钟
#define         COG12864_SDA_PIN         GPIOD,GPIO_Pin_4        //串行数据


#define         CS0_OUT_1       GPIO_SetBits(COG12864_CS0_PIN)
#define         CS0_OUT_0       GPIO_ResetBits(COG12864_CS0_PIN)

#define         RST_OUT_1       GPIO_SetBits(COG12864_RST_PIN)
#define         RST_OUT_0       GPIO_ResetBits(COG12864_RST_PIN)

#define         SDA_OUT_1       GPIO_SetBits(COG12864_SDA_PIN)
#define         SDA_OUT_0       GPIO_ResetBits(COG12864_SDA_PIN)

#define         RS_OUT_1        GPIO_SetBits(COG12864_RS_PIN)
#define         RS_OUT_0        GPIO_ResetBits(COG12864_RS_PIN)

#define         CLK_OUT_1       GPIO_SetBits(COG12864_CLK_PIN)
#define         CLK_OUT_0       GPIO_ResetBits(COG12864_CLK_PIN)


#define         DELAY_US(x)     LSI_delayus(x)
#define         DELAY_MS(x)     LSI_delayms(x)
#define         DELAY_3US       LSI_delayus(3)
#define         DELAY_6MS       LSI_delayms(6)

#define         COG12864_GPIO_MODE      GPIO_Mode_Out_PP_High_Fast
#define         COG12864_CLK_MODE       GPIO_Mode_Out_PP_High_Fast
/*命令*/
#define         SHOW_OPEN               0XAF    //开显示
#define         SHOW_CLOSE              0XAE    //关显示

#define         MODE_SLEEP              0XAC    //睡眠模式
#define         MODE_NORMAL             0XAD    //正常模式
#define         SET_START               0X40    //起始行
/*
    函数
*/

void COG12864_Init(void);
void COG12864_Display(const int* pdata, u16 w, u16 h, int posX, int posY);
void COG12864_Display_add_(const int* pdata, u16 w, u16 h, int posX, int posY);//加下划线的打点
void transfer_command(u8 data1);
void COG12864_GPIO_Init();
void clear_screen(void);        //清屏
void COG12864_Sleep(void);
void COG12864_Wakeup(void);
#endif