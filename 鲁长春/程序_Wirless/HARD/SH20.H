/********************************************************************************
*Author : 
*Time   :  2017年11月19日10:49:43
*Explain: 温湿度传感器SH20驱动
*
********************************************************************************/

#ifndef         __SH20_H
#define         __SH20_H
#include "UHEAD.H"
#include "IIC.H"


#define         BASE_ADDR               0X80                            //器件地址 

//命令
#define         SH20_HOST_TEST_T        0XE3            //SH20做主机测量温度
#define         SH20_HOST_TEST_RH       0XE5            //SH20做主机测量湿度
#define         SH20_SLAVE_TEST_T       0XF3            //SH20做从机测量温度
#define         SH20_SLAVE_TEST_RH      0XF5            //SH20做从机测量湿度
#define         SH20_SOFT_REST          0XFE            //SH20软复位
#define         SH20_USER_W             0XE6            //写用户寄存器
#define         SH20_USER_R             0XE7            //读用户寄存器

#define SH20_DELAY15MS  LSI_delayms(15);

#define SH20_DUGE_BUSY()      //while(CLKPIN_READ_IN == 0); //等待释放CLK

void SH20_Init();
void SH20_Stop();
void SH20_StartMeasure(u8 comd);
answerType SH20_ReadMeasure(s16* datbuf);       
void SH20_Rest();//软复位
answerType SH20_SlaveMeasure(u8 comd,s16* datbuf);//非主机模式下读
void SH20_LowPower(void);

#endif