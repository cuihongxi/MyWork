#ifndef _MYCLK_H
#define _MYCLK_H

#include "UHEAD.H"


/*˵��
	1�� 	STM8�ϵ縴λĬ��HSI/8
	2��	 ϵ ͳ �� λ �� �� �� �� �� �� ʱ �� �� �� �� �� �� ״ ̬ �� �� �� �� ͨ �� �� �� CLK_PCKENR1 ��
		CLK_PCKENR2�е�PCKENλ���ر���Ӧ������ʱ�ӡ������ڹر������ʱ��ǰ���û�����������Ӧ��λ���ø����衣
		Ϊ��ʹ��һ�����裬�û����������üĴ���CLK_PCKENR�ж�Ӧ��PCKENλ��Ȼ������������ƼĴ����е�����ʹ��λ��
        3��	�����ڼĴ���������HSI������ֵ	
	4��	��Դ����
		Ĭ���������ϵͳ���Դ��λ��MCU��������ģʽ��������ģʽ�£�CPU��f CPU �ṩʱ�Ӳ�
		ִ�г�����룬ϵͳʱ�ӷֱ�Ϊ�������ڼ���״̬�������ṩʱ�ӣ�MCU�������
		������ģʽ�£�Ϊ�˱���CPU�������в�ִ�д��룬�����м���;���ɽ��͹������ģ�
			�� ����ϵͳʱ��
			�� �ر�δʹ�������ʱ��
			�� �ر�����δʹ�õ�ģ�⹦�ܿ�
		���ǣ����CPU����Ҫ�������У���ʹ���������ֵ͹���ģʽ��
			�� �ȴ�(Wait)                                      //������ģʽ��ִ��WFI(�ȴ��ж�)ָ��ɽ���ȴ�ģʽ
			�� ��Ծͣ��(Active Halt)(������Ϊ���ٻ���ٻ���)    //Ϊ�����Ծͣ��ģʽ��������ʹ��AWU(��AWU�½�����)��Ȼ��ִ��HALTָ�
                          
			�� ͣ��(Halt) (������Ϊ���ٻ���ٻ���)             //MCU��ͨ��ִ��HALTָ�����ͣ��ģʽ��
*/


#define CLK_BaseAddress         0x50C0


//����ʱ��:PCKENR1

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

//����ʱ�ӣ�PCKENR2
#define Peripheral2_CAN 0X80
#define Peripheral2_ADC 0X08
#define Peripheral2_AWU 0X04

#define IS_Peripheral2_SOURCE(P2_SOURCE)  (((P2_SOURCE) == Peripheral2_CAN) || \
                                           ((P2_SOURCE) == Peripheral2_ADC) || \
                                           ((P2_SOURCE) == Peripheral2_AWU)) 
////////////////////////////////////////////////////////////////////////////////////////
typedef enum{
    HIS_DIV1 = 0x10,       //f HSI  = f HSI RC���
    HIS_DIV2 = 0x11,       //RC���/2
    HIS_DIV4 = 0x12,       //RC���/4
    HIS_DIV8 = 0x13,       //RC���/8

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



//����
void CLK_INIT(CLK_CKDIVR_HIS_CPU HIS_clk,CLK_CKDIVR_HIS_CPU CUP_clk);
void CLOSE_CLK_ALL(void);
void OPEN_CLK1(unsigned char Clk_Peripheral1);//Peripheral1_I2C,Peripheral1_SPI,Peripheral1_UART1,Peripheral1_UART23
                                                //Peripheral1_TIM4,Peripheral1_TIM3,Peripheral1_TIM2,Peripheral1_TIM1
void OPEN_CLK2(unsigned char Clk_Peripheral2);//Clk_Peripheral2��Peripheral2_CAN��Peripheral2_ADC��Peripheral2_AWU

void CLOSE_CLK1(unsigned char Clk_Peripheral1);
void CLOSE_CLK2(unsigned char Clk_Peripheral2);
void MyCLK_AdjustHSICalibrationValue(unsigned char CLK_HSICalibrationValue);              //HSIʱ�������Ĵ���
void MyCLK_SYSCLKConfig(CLK_CKDIVR_HIS_CPU CLK_Prescaler);       //����HSI����CPU�ķ�Ƶ��
                                             
#endif