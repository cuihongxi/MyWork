
#include "24l01.h"


#ifdef DMA_SPI
#include "stm8s_spi.h"
#endif
u8 RF_CH_HZ =10;                                  //Ƶ��0~125
u8  ADDRESS1[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
u8  ADDRESS2[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 
u8* address = ADDRESS1;

#if     DEBUG_24L01 > 0
u8 arraybuf[5] = {0};
#endif
/*****************SPIʱ����******************************************/
u8 SPI2_ReadWriteByte(unsigned char date)
{
#ifdef DMA_SPI
     while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
        SPI_SendData(date);
     while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
        return SPI_ReceiveData();   
#else
    unsigned char i;
   	for(i=0;i<8;i++)                // ѭ��8��
   	{
	  if(date&0x80)
	    MOSI_OUT_1;
	  else
	    MOSI_OUT_0;                 // byte���λ�����MOSI
   	  date<<=1;                     // ��һλ��λ�����λ 
   	  SCLK_OUT_1;       
	  if(READ_MISO_IN)              // ����SCK��nRF24L01��MISO����1λ���ݣ�ͬʱ��MOSI���1λ����
   	   date|=0x01;       	        // ��MISO��byte���λ
   	  SCLK_OUT_0;            	        // SCK�õ�
 
   	}
   return(date);           	// ���ض�����һ�ֽ�    
#endif

}



	  
//��ʼ��24L01��IO��
void NRF24L01_GPIO_Init(void)
{ 	
  //���ų�ʼ��
    GPIO_Init(NRF24L01_CE_PIN,NRF_GPIO_OUTPUTMODE);       	//ʹ��24L01
    GPIO_Init(NRF24L01_IRQ_PIN,NRF_GPIO_INPUTMODE);      				//,��IRQΪ�͵�ƽʱΪ�жϴ���
    GPIO_Init(NRF24L01_CSN_PIN,NRF_GPIO_OUTPUTMODE);     	//SPIƬѡȡ��
    
    GPIO_Init(MOSI_PIN,NRF_GPIO_OUTPUTMODE);    
    GPIO_Init(MISO_PIN,NRF_GPIO_INPUTMODE);
    GPIO_Init(SCLK_PIN,NRF_GPIO_OUTPUTMODE);
   // NRF24L01_GPIO_IRQ();
	
#ifdef DMA_SPI
    SPI_DeInit(); 
    SPI_Init(SPI_FIRSTBIT_MSB, 
              SPI_BAUDRATEPRESCALER_2, 
              SPI_MODE_MASTER, 
              SPI_CLOCKPOLARITY_LOW, 
              SPI_CLOCKPHASE_1EDGE, 
              SPI_DATADIRECTION_2LINES_FULLDUPLEX, 
              SPI_NSS_SOFT,7);
    SPI_Cmd(ENABLE); 
#endif
 


}

//ʹ��DPL��̬����
//pipNum ͨ����
void NRF24L01_EnabelDPL(u8 pipNum)
{
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,(1<<pipNum));                //�Զ�Ӧ�� 
	NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, (1<<FEATURE_BIT_EN_DPL)|(1<<FEATURE_BIT_EN_ACK_PAY));	//ʹ�ܶ�̬����
	NRF24L01_Write_Reg(NRF_WRITE_REG + DYNPD, (1<<pipNum));	//ʹ��ͨ��0��̬���� ,Requires EN_DPL and ENAA_P0
}
//��ʼ������
void Init_NRF24L01(u8 pip,u8 rf_ch)
{
    NRF24L01_GPIO_Init();
     while(NRF24L01_Check())         //���ģ�����,��������ھ�����1s�л��̵���״̬,��LED��˸
    {            
        debug("NRF24L01_Check EEROR\r\n"); 
        delay_ms(1000);
    }
    NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,address,TX_ADR_WIDTH); ;    //д���ص�ַ	
    NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,address,RX_ADR_WIDTH); //д���ն˵�ַ
    NRF24L01_EnabelDPL(BIT_PIP0);									//ʹ��ͨ��0�Զ�Ӧ�𣬶�̬����
    NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,(0x01<<pip));            //������յ�ַƵ��0 
    NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,(REPEAT_DELAY<<4)|REPEAT_TIME); //�����Զ��ط����ʱ��;����Զ��ط�����
    NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,rf_ch);            //�����ŵ�����Ƶ�ʣ��շ�����һ��
    NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,RF_SETUP_DAT);// NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x6f);   //SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f); //���÷�������Ϊ2MHZ�����书��Ϊ���ֵ0dB	
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x7c); //���û�������ģʽ�Ĳ���;PWR_UP=0,EN_CRC,16BIT_CRC,����ģʽ,�������ж�	
	
}




//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
  u8 buf[2]={0XA5,0x33};   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,2);        
        buf[0] = buf[1] = 0;	
	NRF24L01_Read_Buf(TX_ADDR,buf,2);                       //����д��ĵ�ַ  
	if(buf[0]!=0XA5)return 1;                                  //���24L01����	
        //printf("buf = %#X,%#X\r\n",buf[0],buf[1]);
	return 0;		                                //��⵽24L01
}

//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;
    SCLK_OUT_0;    
   	CSN_OUT_0;                              //ʹ��SPI����
  	status =SPI2_ReadWriteByte(reg);        //���ͼĴ����� 
  	SPI2_ReadWriteByte(value);              //д��Ĵ�����ֵ
         
  	CSN_OUT_1;                              //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;
        SCLK_OUT_0;
 	CSN_OUT_0;                              //ʹ��SPI����		
  	SPI2_ReadWriteByte(reg);                //���ͼĴ�����
  	reg_val=SPI2_ReadWriteByte(0XFF);       //��ȡ�Ĵ�������
  	CSN_OUT_1;                              //��ֹSPI����		    
  	return(reg_val);                        //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;
        SCLK_OUT_0;
  	CSN_OUT_0;                              //ʹ��SPI����
  	status=SPI2_ReadWriteByte(reg);         //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
  	CSN_OUT_1;                              //�ر�SPI����
  	return status;                          //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;
    SCLK_OUT_0 ;
 	CSN_OUT_0;                              //ʹ��SPI����
  	status = SPI2_ReadWriteByte(reg);       //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������
  	CSN_OUT_1;                              //�ر�SPI����
  	return status;                          //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//size:���ݵĸ���
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf,u8 size)
{  	
    SCLK_OUT_0 ;
	CE_OUT_0;                               //StandBy Iģʽ	
	if(NRF24L01_Read_Reg(NRF_FIFO_STATUS) &(1<<FIFO_TX_FULL))	
	{
		CE_OUT_0;
		
		NRF24L01_Write_Reg(FLUSH_TX,0x00); //���tx fifo�Ĵ���	//********��Ҫ*********	
		CE_OUT_1;
	}
			
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,size);
 	CE_OUT_1;                               //�������� �ø�CE�������ݷ���    
	return 0;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf,u8* len)
{
 
	u8 sta;		    							      
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ      
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
	  	*len = NRF24L01_GetRXLen();
		debug("len = %d  sta = %x\r\n",*len,sta);
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		u8 txbuf[2] = {0x30,0x31};
		 NRF24L01_RX_AtuoACKPip(txbuf,2,BIT_PIP0);
		 if(NRF24L01_Read_Reg(NRF_FIFO_STATUS) &(1<<FIFO_RX_FULL))
		 {
		 	CE_OUT_0;
			NRF24L01_Write_Reg(FLUSH_RX,0x00);//���RX FIFO�Ĵ��� 
			CE_OUT_1;
		 }
		
               
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{  
 		debug("SLAVE_Mode\r\n");
        CE_OUT_0; 
        NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,0xff);	//����жϱ�־
        NRF24L01_Write_Reg(FLUSH_RX,0x00); 			//���RX_FIFO�Ĵ���
		NRF24L01_Write_Reg(FLUSH_TX,0x00);	        //���TX_FIFO�Ĵ��� 
        NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);//IRQ���Ų���ʾ�ж� �ϵ� ����ģʽ   1~16CRCУ��   
        CE_OUT_1; 
        DELAY_130US(); //��CE = 0 �� CE = 1��������ģʽ���շ�ģʽ����Ҫ���130us
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{	
    debug("TX_Mode\r\n");
        CE_OUT_0; 
        NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,address,TX_ADR_WIDTH);     //д���ص�ַ
        NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,0xff); 	//����жϱ�־
		NRF24L01_Write_Reg(FLUSH_RX,0x00); 			//���RX_FIFO�Ĵ���
        NRF24L01_Write_Reg(FLUSH_TX,0x00);	        //���TX_FIFO�Ĵ��� 
        NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG,0x0e);    //IRQ���Ų���ʾTX,MAX�ж�,��ʾRX�ж� �ϵ� ����ģʽ  1~16CRCУ��

        CE_OUT_1;
        DELAY_130US();//��CE = 0 �� CE = 1��������ģʽ���շ�ģʽ����Ҫ���130us	
    //  CE_OUT_0; 
}

//1��0�رյ�Դ
void NRF24L01_PWR(u8 state)
{
    u8 config = NRF24L01_Read_Reg(CONFIG);
    if(state) NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,config|0x02);
    else NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,config&0xFD);

}

//���ý���Ƶ��
void NRF24L01_SetRXHZ(u8 hz)
{
  CE_OUT_0; 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,hz);
  CE_OUT_1;

 // NRF24L01_RX_Mode();                         //���ý���ģʽ        
}

//���÷���Ƶ��
void NRF24L01_SetTXHZ(u8 hz)
{ 
  CE_OUT_0; 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,hz);
  CE_OUT_1;

}

//��ȡ����RX����
u8 NRF24L01_GetRXLen(void)
{
    SCLK_OUT_0;    
   	CSN_OUT_0;                              //ʹ��SPI����
  	SPI2_ReadWriteByte(R_RX_PL_WID);        //���ͼĴ����� 
  	u8 dat =SPI2_ReadWriteByte(0xff);      //д��Ĵ�����ֵ
  	CSN_OUT_1;                              //��ֹSPI����
	
	return dat;
}

//RX ACK �Զ��ظ�������ͨ��
void NRF24L01_RX_AtuoACKPip(u8 *txbuf,u8 size,u8 pip)
{
	 NRF24L01_Write_Buf(W_ACK_PAYLOAD|pip,txbuf,size);
}

