#ifndef _IIC_H
#define _IIC_H

#include "UHEAD.H"

#define         DEBUG_IIC     0
#if  DEBUG_IIC > 0
#include "stm8l15x_usart.h"
#endif

/*
*SCL高电平时,SDA保持数据稳定,SCL低电平时,SDA变化
*/

//#define         BASE_ADDR               0X40                            //器件地址  
#define         DELAY_20MS              LSI_delayus(40)
#define         DELAY_2US               // LSI_delayus(2)

#define         SDA_PIN                 GPIOC,GPIO_Pin_0
#define         SCL_PIN                 GPIOC,GPIO_Pin_1

#define         SCL_PIN_LOW             GPIO_ResetBits(SCL_PIN)         //拉低SCL电平
#define         SCL_PIN_HIGH            GPIO_SetBits(SCL_PIN)           //拉低SCL电平
#define         SDA_PIN_LOW             GPIO_ResetBits(SDA_PIN)
#define         SDA_PIN_HIGH            GPIO_SetBits(SDA_PIN)

#define         IIC_SDAGPIO_MODE        GPIO_Mode_Out_OD_HiZ_Slow       //GPIO引脚模式
#define         IIC_CLKGPIO_MODE        GPIO_Mode_Out_OD_HiZ_Slow 


#define         SDAPIN_READ_IN          GPIO_ReadInputDataBit(SDA_PIN)
#define         CLKPIN_READ_IN          GPIO_ReadInputDataBit(SCL_PIN)

//配置管脚输入出属性
#define         SCL_OUT                 GPIO_Init(SCL_PIN,IIC_CLKGPIO_MODE)
#define         SDA_OUT                 GPIO_Init(SDA_PIN,IIC_SDAGPIO_MODE)
#define         SDA_IN                 // GPIO_Init(SDA_PIN,GPIO_Mode_In_PU_No_IT)


#define         JUGE_BUSY()             //while(SDAPIN_READ_IN == 0)      
typedef enum 
{
  N_ANSWER,  
  ANSWER,    
}answerType;




void            IIC_Init();
void            IIC_Start();                                       //起始信号
void            IIC_WriteByte(u8 byte);                            //写数据
u8              IIC_ReadByte();                                    //读数据
answerType      IIC_Response(void);                                //I2C从机应答
void            IIC_MasterResponse(answerType answer);             //I2C主机应答
void            IIC_Stop(void);                                    //停止

void            IIC_WriteREGByte(u16 base_addr,u8 reg);            //单字节写入


#endif