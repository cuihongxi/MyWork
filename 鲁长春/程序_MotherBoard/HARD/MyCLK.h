#ifndef _MYCLK_H
#define _MYCLK_H

#include "UHEAD.H"


/*说明
	1： 	STM8上电复位默认HSI/8
	2：	 系 统 复 位 后 ， 所 有 外 设 时 钟 均 处 于 开 的 状 态 。 用 户 可 通 过 清 除 CLK_PCKENR1 或
		CLK_PCKENR2中的PCKEN位来关闭相应的外设时钟。但是在关闭外设的时钟前，用户必须设置相应的位禁用该外设。
		为了使能一个外设，用户必须先设置寄存器CLK_PCKENR中对应的PCKEN位，然后设置外设控制寄存器中的外设使能位。
        3：	可以在寄存器中配置HSI的修正值	
	4：	电源管理
		默认情况下在系统或电源复位后，MCU处于运行模式。在这种模式下，CPU由f CPU 提供时钟并
		执行程序代码，系统时钟分别为各个处于激活状态的外设提供时钟，MCU功耗最大。
		在运行模式下，为了保持CPU继续运行并执行代码，有下列几种途径可降低功率消耗：
			● 降低系统时钟
			● 关闭未使用外设的时钟
			● 关闭所有未使用的模拟功能块
		但是，如果CPU不需要保持运行，可使用下列三种低功耗模式：
			● 等待(Wait)                                      //在运行模式下执行WFI(等待中断)指令，可进入等待模式
			● 活跃停机(Active Halt)(可配置为慢速或快速唤醒)    //为进入活跃停机模式，需首先使能AWU(如AWU章节所述)，然后执行HALT指令。
                          
			● 停机(Halt) (可配置为慢速或快速唤醒)             //MCU可通过执行HALT指令进入停机模式。
*/


#define CLK_BaseAddress         0x50C0


//外设时钟:PCKENR1

#define Peripheral1_I2C          0x01
#define Peripheral1_SPI          0x02
#define Peripheral1_UART1        0x04
#define Peripheral1_UART23       0x08
#define Peripheral1_TIM4         0x10
#define Peripheral1_TIM3         0x20
#define Peripheral1_TIM2         0x40
#define Peripheral1_TIM1         0x80

#define IS_Peripheral1_SOURCE(P1_SOURCE)  (((P1_SOURCE) == Peripheral1_I2C) || \
                                           ((P1_SOURCE) == Peripheral1_SPI) || \
                                           ((P1_SOURCE) == Peripheral1_UART1) || \
                                           ((P1_SOURCE) == Peripheral1_UART23) || \
                                           ((P1_SOURCE) == Peripheral1_TIM4) || \
                                           ((P1_SOURCE) == Peripheral1_TIM3) || \
                                           ((P1_SOURCE) == Peripheral1_TIM2) || \
                                           ((P1_SOURCE) == Peripheral1_TIM1))      

//外设时钟：PCKENR2
#define Peripheral2_CAN 0X80
#define Peripheral2_ADC 0X08
#define Peripheral2_AWU 0X04

#define IS_Peripheral2_SOURCE(P2_SOURCE)  (((P2_SOURCE) == Peripheral2_CAN) || \
                                           ((P2_SOURCE) == Peripheral2_ADC) || \
                                           ((P2_SOURCE) == Peripheral2_AWU)) 
////////////////////////////////////////////////////////////////////////////////////////
typedef enum{
    HIS_DIV1 = 0x10,       //f HSI  = f HSI RC输出
    HIS_DIV2 = 0x11,       //RC输出/2
    HIS_DIV4 = 0x12,       //RC输出/4
    HIS_DIV8 = 0x13,       //RC输出/8

    CPU_DIV1 = 0,       //f CPU  = f MASTER
    CPU_DIV2 = 1,       //f CPU  = f MASTER /2
    CPU_DIV4 = 2,       //f CPU  = f MASTER /4
    CPU_DIV8 = 3,       //f CPU  = f MASTER /8
    CPU_DIV16 =4,       //f CPU  = f MASTER /16
    CPU_DIV32 =5,       //f CPU  = f MASTER /32
    CPU_DIV64 =6,       //f CPU  = f MASTER/ 64
    CPU_DIV128=7,       //f CPU  = f MASTER /128                                         
}CLK_CKDIVR_HIS_CPU;

typedef struct
{
  unsigned char HSITRIM     : 4;
} __BITS_CLK_HSITRIMR;

typedef struct
{
  unsigned char CPUDIV      : 3;
  unsigned char HSIDIV      : 2;
} __BITS_CLK_CKDIVR;



//函数
void CLK_INIT(CLK_CKDIVR_HIS_CPU HIS_clk,CLK_CKDIVR_HIS_CPU CUP_clk);
void CLOSE_CLK_ALL(void);
void OPEN_CLK1(unsigned char Clk_Peripheral1);//Peripheral1_I2C,Peripheral1_SPI,Peripheral1_UART1,Peripheral1_UART23
                                                //Peripheral1_TIM4,Peripheral1_TIM3,Peripheral1_TIM2,Peripheral1_TIM1
void OPEN_CLK2(unsigned char Clk_Peripheral2);//Clk_Peripheral2：Peripheral2_CAN，Peripheral2_ADC，Peripheral2_AWU

void CLOSE_CLK1(unsigned char Clk_Peripheral1);
void CLOSE_CLK2(unsigned char Clk_Peripheral2);
void MyCLK_AdjustHSICalibrationValue(unsigned char CLK_HSICalibrationValue);              //HSI时钟修正寄存器
void MyCLK_SYSCLKConfig(CLK_CKDIVR_HIS_CPU CLK_Prescaler);       //配置HSI或者CPU的分频器
                                             
#endif