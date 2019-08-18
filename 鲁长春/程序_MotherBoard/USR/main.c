#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "ADC_CHECK.H"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"

Nrf24l01_PTXStr ptx = {0};

#define SIZE_TX 3
u8 rxbuf[32] = {0};
u8 txbuf[SIZE_TX] = {0};

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
	UART_INIT(115200);
	InitNRF_AutoAck_PTX(&ptx,rxbuf,sizeof(rxbuf),BIT_PIP0,RF_CH_HZ);
	LED_GPIO_Init();   										// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                                 	// ���IO����
	
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
	GPIO_ADC_Init();
	for(u8 i=0;i<SIZE_TX;i++)
	  txbuf[i] = i;
	
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
		
		NRF24L01_PTXInMain(&ptx,txbuf,sizeof(txbuf)); // �������еķ��ͺ���
	}
}

//IRQ �жϷ�����

INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	ptx.IRQCallBack(&ptx);
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

