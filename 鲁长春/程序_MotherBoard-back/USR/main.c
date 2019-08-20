#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "ADC_CHECK.H"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "lowpower.h"

Nrf24l01_PRXStr prx = {0};		//NRF���սṹ��
u8 txbuf[] = {1};				//nrf���ͻ���
u8 rxbuf[100] = {0};			//nrf���ջ���


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
	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_GPIO_Init();   										// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                                 	// ���IO����
	
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
	GPIO_ADC_Init();
	
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
		
		/*nrf���պ���*/
		if(prx.hasrxlen != 0)
		{
		  debug("hasrxlen = %d :\r\n",prx.hasrxlen);		
			for(u8 i=0;i<prx.hasrxlen;i++)
			  {
				debug("rxbuf[%d]=%d	",i,prx.rxbuf[i]);
			  }
			
			debug("\r\n##################################\r\n");
			
			prx.hasrxlen = 0;
		}
	}
}

//IRQ �жϷ�����

INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	prx.IRQCallBack(&prx);
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

