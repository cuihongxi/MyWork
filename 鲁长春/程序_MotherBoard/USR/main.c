#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "ADC_CHECK.H"
#include "24l01.h"

u8 dat[3] = {0x1,0x2,3};
//低功耗时钟和GPIO初始化
void CLK_GPIO_Init()
{
    CLK_INIT(HIS_DIV1,CPU_DIV1);                                            
    CLK_HSICmd(ENABLE);   
}

void LSI_delayus(unsigned int  nCount)   //16M 晶振时  延时 1个微妙
{
    while(--nCount);
}

void LSI_delayms(unsigned int  nCount)  //16M 晶振时  延时 1个毫秒
{
    while(nCount--)
    {
        LSI_delayus(1000);
    }
}


void main()
{
  	CLK_GPIO_Init();										// 低功耗时钟和GPIO初始化
	LED_GPIO_Init();   										// 双色LED初始化
	MX830Motor_GPIOInit();                                 	// 马达IO配置
	UART_INIT(115200);
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
	GPIO_ADC_Init();

	Init_NRF24L01();
	NRF24L01_TX_Mode();				// 配置为发送模式
	
	while(1)
	{
	    
		//debug("YS_Channel: %d\r\n",Get_ADC_Dat(YS_Channel));
		delay_ms(1000);
		GPIO_RESET(LED1_GREENGPIO);
		GPIO_SET(LED2_REDGPIO);
		//debug("Battery_Channel: %d\r\n",Get_ADC_Dat(Battery_Channel));
		delay_ms(1000);
		GPIO_SET(LED1_GREENGPIO);
		GPIO_RESET(LED2_REDGPIO);
		
		NRF24L01_TxPacket(dat,3);  //发送新地址到接收板
		dat[0] ++;
		dat[1] ++;
		dat[2] ++;
	}
}


