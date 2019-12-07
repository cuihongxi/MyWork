/*
*   NRF24L01
* ע��:��Դ�͵�֮��Ҫ�Ӹ��Դ��ĵ���
    ��Ҫ����TXRX�ĳ���
	2019-8-16 ���£�
		��������24L01ģ���ѡ�񣬸���ѡ������ RF_SETUP_DAT
*/

#ifndef __24L01_H
#define __24L01_H

#include "UHEAD.H"


//NRF24L01�Ĵ�����������
#define NRF_READ_REG    0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG   0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                              //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)Ϊ1ʹ��;bit5:�ж�TX_DSΪ1ʹ��;bit6:�ж�RX_DRΪ1ʹ��
#define EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5

#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                              //bit5:���ݷ�������ж�;bit6:���������ж�;
	#define	STATUS_BIT_IRT_RPTX		4		//bit4:�ﵽ�����ط��ж�
	#define	STATUS_BIT_IRT_TXD		5		//bit5:���ݷ�������ж�
	#define	STATUS_BIT_IRT_RXD		6		//bit6:���������ж�

#define MAX_TX  	0x10  //�ﵽ����ʹ����ж�
#define TX_OK   	0x20  //TX��������ж�
#define RX_OK   	0x40  //���յ������ж�

#define OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF_FIFO_STATUS 0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                              //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;
	#define FIFO_RX_FULL	1	//���ռĴ�����
	#define FIFO_TX_FULL	5	//����Ĵ�����
/********************2019-8-16���ӼĴ���*********************************/
#define R_RX_PL_WID  	0x60  	// ��ѯ��ǰ������ݵ���Ч����
#define	DYNPD			0X1C	// ʹ�ܶ�̬����
	#define	BIT_PIP0	0		// ��������ͨ��0 �ı���λ
	#define	BIT_PIP1	1		// ��������ͨ��1 �ı���λ
	#define	BIT_PIP2	2		// ��������ͨ��2 �ı���λ
	#define	BIT_PIP3	3		// ��������ͨ��3 �ı���λ
	#define	BIT_PIP4	4		// ��������ͨ��4 �ı���λ
	#define	BIT_PIP5	5		// ��������ͨ��5 �ı���λ
#define	FEATURE			0x1d	// �����Ĵ���
	#define	FEATURE_BIT_EN_DPL		2		//ʹ�ܶ�̬�غɳ���
	#define	FEATURE_BIT_EN_ACK_PAY	1		//ʹ����Ч�غ�Ӧ��
	#define	FEATURE_BIT_EN_DYN_ACK	0		//ʹ�� W_TX_PAYLOAD_NOACK ����

#define	W_ACK_PAYLOAD 	0xA8	// �����Զ��ظ���ͨ����

//24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32  	//6�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32  	//6�ֽڵ��û����ݿ��

/********************2019-8-16���ӹ���*********************************/
#define USE_NRF_SI24

#ifdef  USE_NRF_MINI
#define RF_SETUP_DAT    0X0F    //���÷�������Ϊ2MHZ�����书��Ϊ���ֵ0dB
#endif

#ifdef  USE_NRF_SI24
#define RF_SETUP_DAT    0x0e	//���÷�������Ϊ2MHZ�����书��Ϊ���ֵ0dB
#endif

//��Ƶ������-SI24
#define	RF_DR_1M	0	
#define	RF_DR_2M	0x01
#define	RF_DR_250K	0x02

//��Ƶ���͹���-SI24
#define	RF_PWR_7dBm		0x07
#define	RF_PWR_4dBm		0x06
#define	RF_PWR_3dBm		0x05
#define	RF_PWR_1dBm		0x04
#define	RF_PWR_0dBm		0x03
#define	RF_PWR_sub_4dBm		0x02
#define	RF_PWR_sub_6dBm		0x01
#define	RF_PWR_sub_12dBm	0x00

#define	VALUE_RF_SETUP(dr,pwr)	(pwr|(((dr&0x01)<<3)|(((dr>>1)<<5))))

/*���º궨��Ϊ����ֲ���������*/		


//#define DMA_SPI

//ģ������Ŷ���
#define NRF24L01_CE_PIN         GPIOD,GPIO_Pin_3        //RX/TXģʽѡ���
#define NRF24L01_IRQ_PIN        GPIOD,GPIO_Pin_2        //�������ж϶�
#define NRF24L01_CSN_PIN        GPIOC,GPIO_Pin_5        //SPIƬѡ��
#define MOSI_PIN                GPIOA,GPIO_Pin_3        //SPI��������ӻ������
#define MISO_PIN                GPIOA,GPIO_Pin_2        //SPI��������ӻ������
#define SCLK_PIN                GPIOC,GPIO_Pin_6        //SPIʱ�Ӷ�




#define		NRF_GPIO_OUTPUTMODE		GPIO_Mode_Out_PP_Low_Fast
#define		NRF_GPIO_INPUTMODE		GPIO_Mode_In_PU_No_IT


#define DELAY_10US()            delay_us(10)  
#define DELAY_130US()           delay_us(130) 



#define MOSI_OUT_1      GPIO_SET(MOSI_PIN)
#define MOSI_OUT_0      GPIO_RESET(MOSI_PIN)
#define READ_MISO_IN    GPIO_ReadInputDataBit(MISO_PIN)
#define SCLK_OUT_1      GPIO_SET(SCLK_PIN)
#define SCLK_OUT_0      GPIO_RESET(SCLK_PIN)
#define CSN_OUT_0       GPIO_RESET(NRF24L01_CSN_PIN)
#define CE_OUT_0        GPIO_RESET(NRF24L01_CE_PIN)
#define CE_OUT_1        GPIO_SET(NRF24L01_CE_PIN)
#define READ_IRQ_IN     GPIO_ReadInputDataBit(NRF24L01_IRQ_PIN)
#ifdef  DMA_SPI
#define CSN_OUT_1        while(SPI_GetFlagStatus(SPI_FLAG_BSY));GPIO_SET(NRF24L01_CSN_PIN)
#else 
#define CSN_OUT_1       GPIO_SET(NRF24L01_CSN_PIN)
#endif

#define REPEAT_TIME     15      //�ط�����
#define	REPEAT_DELAY	15		//�ظ��������λ250uS

extern u8 RF_CH_HZ ; 
extern u8  ADDRESS1[TX_ADR_WIDTH]; //���͵�ַ
extern u8  ADDRESS2[RX_ADR_WIDTH]; 
extern u8* address;
/*����*/
void NRF24L01_GPIO_Init(void);					//��ʼ��
u8 Init_NRF24L01(u8 pip,u8 rf_ch);             //��ʼ��
void NRF24L01_RX_Mode(void);					//����Ϊ����ģʽ
void NRF24L01_TX_Mode(void);					//����Ϊ����ģʽ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 u8s);                //д������
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 u8s);	                //��������		  
u8 NRF24L01_Read_Reg(u8 reg);							//���Ĵ���
u8 NRF24L01_Write_Reg(u8 reg, u8 value);		        //д�Ĵ���
u8 NRF24L01_Check(void);								//���24L01�Ƿ����
u8 NRF24L01_TxPacket(u8 *txbuf,u8 size);				//����һ����������
u8 NRF24L01_RxPacket(u8 *rxbuf,u8* len);				//����һ����������
void NRF24L01_PWR(u8 state);                            //1��0�رյ�Դ
void FreeNRFGPIO();
//����Ƶ��
void NRF24L01_SetRXHZ(u8 hz);
void NRF24L01_SetTXHZ(u8 hz);

/********************2019-8-16���Ӻ���*********************************/
//��ȡ���յ��ĳ�����Ϣ
u8 NRF24L01_GetRXLen(void);
void NRF24L01_RX_AtuoACKPip(u8 *txbuf,u8 size,u8 pip);// RX ACK �Զ��ظ�������ͨ��

/********************2019-8-18���Ӻ���*********************************/

u8 NRF24L01_GetStatus(void);	// ��ȡ״̬status

/********************2019��9��8�����Ӻ���*********************************/

void CreatNewAddr(u8* ChipID,u8* newAddr);				// ��ID�������µ��շ���ַ
void Get_ChipID(u8 *ChipID);
/********************2019��10��4�����Ӻ���*********************************/
void NRF24L01_ResetAddr(u8* add);	//�����ַ
void NRF24L01_SetRF_SETUP(u8 dr,u8 pwr);//������Ƶ�����ʣ����书��
/********************2019��10��6�����Ӻ���*********************************/
void NRF_CreatNewAddr(u8* addr);//����ΨһID������һ���µ�ַ
/********************2019��10��30�����Ӻ���*********************************/
void NRF24L01_GPIO_Lowpower(void);
/********************2019��11��16�����Ӻ���*********************************/
void NRF24L01_RESUSE_TX();	// �ط���һ������
void NRF24L01_ClearFIFO(void);

#endif













