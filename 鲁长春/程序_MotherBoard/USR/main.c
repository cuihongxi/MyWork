#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "ADC_CHECK.H"
#include "24l01.h"

u8 dat[3] = {0x1,0x2,3};
//�͹���ʱ�Ӻ�GPIO��ʼ��
void CLK_GPIO_Init()
{
    CLK_INIT(HIS_DIV1,CPU_DIV1);                                            
    CLK_HSICmd(ENABLE);   
}

void LSI_delayus(unsigned int  nCount)   //16M ����ʱ  ��ʱ 1��΢��
{
    while(--nCount);
}

void LSI_delayms(unsigned int  nCount)  //16M ����ʱ  ��ʱ 1������
{
    while(nCount--)
    {
        LSI_delayus(1000);
    }
}


void main()
{
  	CLK_GPIO_Init();										// �͹���ʱ�Ӻ�GPIO��ʼ��
	LED_GPIO_Init();   										// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                                 	// ���IO����
	UART_INIT(115200);
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
	GPIO_ADC_Init();

	Init_NRF24L01();
	NRF24L01_TX_Mode();				// ����Ϊ����ģʽ
	
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
		
		NRF24L01_TxPacket(dat,3);  //�����µ�ַ�����հ�
		dat[0] ++;
		dat[1] ++;
		dat[2] ++;
	}
}


