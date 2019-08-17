
#include "24l01.h"


#ifdef DMA_SPI
#include "stm8s_spi.h"
#endif
u8 RF_CH_HZ =10;                                  //频率0~125
u8  ADDRESS1[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
u8  ADDRESS2[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 
u8* address = ADDRESS1;

#if     DEBUG_24L01 > 0
u8 arraybuf[5] = {0};
#endif
/*****************SPI时序函数******************************************/
u8 SPI2_ReadWriteByte(unsigned char date)
{
#ifdef DMA_SPI
     while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
        SPI_SendData(date);
     while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
        return SPI_ReceiveData();   
#else
    unsigned char i;
   	for(i=0;i<8;i++)                // 循环8次
   	{
	  if(date&0x80)
	    MOSI_OUT_1;
	  else
	    MOSI_OUT_0;                 // byte最高位输出到MOSI
   	  date<<=1;                     // 低一位移位到最高位 
   	  SCLK_OUT_1;       
	  if(READ_MISO_IN)              // 拉高SCK，nRF24L01从MISO读入1位数据，同时从MOSI输出1位数据
   	   date|=0x01;       	        // 读MISO到byte最低位
   	  SCLK_OUT_0;            	        // SCK置低
 
   	}
   return(date);           	// 返回读出的一字节    
#endif

}



	  
//初始化24L01的IO口
void NRF24L01_GPIO_Init(void)
{ 	
  //引脚初始化
    GPIO_Init(NRF24L01_CE_PIN,NRF_GPIO_OUTPUTMODE);       	//使能24L01
    GPIO_Init(NRF24L01_IRQ_PIN,NRF_GPIO_INPUTMODE);      				//,当IRQ为低电平时为中断触发
    GPIO_Init(NRF24L01_CSN_PIN,NRF_GPIO_OUTPUTMODE);     	//SPI片选取消
    
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

//使能DPL动态长度
//pipNum 通道号
void NRF24L01_EnabelDPL(u8 pipNum)
{
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,(1<<pipNum));                //自动应答 
	NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, (1<<FEATURE_BIT_EN_DPL)|(1<<FEATURE_BIT_EN_ACK_PAY));	//使能动态长度
	NRF24L01_Write_Reg(NRF_WRITE_REG + DYNPD, (1<<pipNum));	//使能通道0动态长度 ,Requires EN_DPL and ENAA_P0
}
//初始化配置
void Init_NRF24L01(u8 pip,u8 rf_ch)
{
    NRF24L01_GPIO_Init();
     while(NRF24L01_Check())         //检测模块存在,如果不存在就周期1s切换继电器状态,让LED闪烁
    {            
        debug("NRF24L01_Check EEROR\r\n"); 
        delay_ms(1000);
    }
    NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,address,TX_ADR_WIDTH); ;    //写本地地址	
    NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,address,RX_ADR_WIDTH); //写接收端地址
    NRF24L01_EnabelDPL(BIT_PIP0);									//使能通道0自动应答，动态长度
    NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,(0x01<<pip));            //允许接收地址频道0 
    NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,(REPEAT_DELAY<<4)|REPEAT_TIME); //设置自动重发间隔时间;最大自动重发次数
    NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,rf_ch);            //设置信道工作频率，收发必须一致
    NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,RF_SETUP_DAT);// NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x6f);   //SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f); //设置发射速率为2MHZ，发射功率为最大值0dB	
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x7c); //配置基本工作模式的参数;PWR_UP=0,EN_CRC,16BIT_CRC,接收模式,不所有中断	
	
}




//检测24L01是否存在
//返回值:0，成功;1，失败	
u8 NRF24L01_Check(void)
{
  u8 buf[2]={0XA5,0x33};   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,2);        
        buf[0] = buf[1] = 0;	
	NRF24L01_Read_Buf(TX_ADDR,buf,2);                       //读出写入的地址  
	if(buf[0]!=0XA5)return 1;                                  //检测24L01错误	
        //printf("buf = %#X,%#X\r\n",buf[0],buf[1]);
	return 0;		                                //检测到24L01
}

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;
    SCLK_OUT_0;    
   	CSN_OUT_0;                              //使能SPI传输
  	status =SPI2_ReadWriteByte(reg);        //发送寄存器号 
  	SPI2_ReadWriteByte(value);              //写入寄存器的值
         
  	CSN_OUT_1;                              //禁止SPI传输	   
  	return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;
        SCLK_OUT_0;
 	CSN_OUT_0;                              //使能SPI传输		
  	SPI2_ReadWriteByte(reg);                //发送寄存器号
  	reg_val=SPI2_ReadWriteByte(0XFF);       //读取寄存器内容
  	CSN_OUT_1;                              //禁止SPI传输		    
  	return(reg_val);                        //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;
        SCLK_OUT_0;
  	CSN_OUT_0;                              //使能SPI传输
  	status=SPI2_ReadWriteByte(reg);         //发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
  	CSN_OUT_1;                              //关闭SPI传输
  	return status;                          //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;
    SCLK_OUT_0 ;
 	CSN_OUT_0;                              //使能SPI传输
  	status = SPI2_ReadWriteByte(reg);       //发送寄存器值(位置),并读取状态值
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据
  	CSN_OUT_1;                              //关闭SPI传输
  	return status;                          //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//size:数据的个数
//返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf,u8 size)
{  	
    SCLK_OUT_0 ;
	CE_OUT_0;                               //StandBy I模式	
	if(NRF24L01_Read_Reg(NRF_FIFO_STATUS) &(1<<FIFO_TX_FULL))	
	{
		CE_OUT_0;
		
		NRF24L01_Write_Reg(FLUSH_TX,0x00); //清除tx fifo寄存器	//********重要*********	
		CE_OUT_1;
	}
			
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,size);
 	CE_OUT_1;                               //启动发送 置高CE激发数据发送    
	return 0;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
u8 NRF24L01_RxPacket(u8 *rxbuf,u8* len)
{
 
	u8 sta;		    							      
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值      
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
	  	*len = NRF24L01_GetRXLen();
		debug("len = %d  sta = %x\r\n",*len,sta);
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		u8 txbuf[2] = {0x30,0x31};
		 NRF24L01_RX_AtuoACKPip(txbuf,2,BIT_PIP0);
		 if(NRF24L01_Read_Reg(NRF_FIFO_STATUS) &(1<<FIFO_RX_FULL))
		 {
		 	CE_OUT_0;
			NRF24L01_Write_Reg(FLUSH_RX,0x00);//清除RX FIFO寄存器 
			CE_OUT_1;
		 }
		
               
		return 0; 
	}	   
	return 1;//没收到任何数据
}					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{  
 		debug("SLAVE_Mode\r\n");
        CE_OUT_0; 
        NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,0xff);	//清除中断标志
        NRF24L01_Write_Reg(FLUSH_RX,0x00); 			//清除RX_FIFO寄存器
		NRF24L01_Write_Reg(FLUSH_TX,0x00);	        //清除TX_FIFO寄存器 
        NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);//IRQ引脚不显示中断 上电 接收模式   1~16CRC校验   
        CE_OUT_1; 
        DELAY_130US(); //从CE = 0 到 CE = 1；即待机模式到收发模式，需要最大130us
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF24L01_TX_Mode(void)
{	
    debug("TX_Mode\r\n");
        CE_OUT_0; 
        NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,address,TX_ADR_WIDTH);     //写本地地址
        NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,0xff); 	//清除中断标志
		NRF24L01_Write_Reg(FLUSH_RX,0x00); 			//清除RX_FIFO寄存器
        NRF24L01_Write_Reg(FLUSH_TX,0x00);	        //清除TX_FIFO寄存器 
        NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG,0x0e);    //IRQ引脚不显示TX,MAX中断,显示RX中断 上电 发射模式  1~16CRC校验

        CE_OUT_1;
        DELAY_130US();//从CE = 0 到 CE = 1；即待机模式到收发模式，需要最大130us	
    //  CE_OUT_0; 
}

//1打开0关闭电源
void NRF24L01_PWR(u8 state)
{
    u8 config = NRF24L01_Read_Reg(CONFIG);
    if(state) NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,config|0x02);
    else NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,config&0xFD);

}

//设置接收频率
void NRF24L01_SetRXHZ(u8 hz)
{
  CE_OUT_0; 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,hz);
  CE_OUT_1;

 // NRF24L01_RX_Mode();                         //配置接收模式        
}

//设置发送频率
void NRF24L01_SetTXHZ(u8 hz)
{ 
  CE_OUT_0; 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,hz);
  CE_OUT_1;

}

//获取接收RX长度
u8 NRF24L01_GetRXLen(void)
{
    SCLK_OUT_0;    
   	CSN_OUT_0;                              //使能SPI传输
  	SPI2_ReadWriteByte(R_RX_PL_WID);        //发送寄存器号 
  	u8 dat =SPI2_ReadWriteByte(0xff);      //写入寄存器的值
  	CSN_OUT_1;                              //禁止SPI传输
	
	return dat;
}

//RX ACK 自动回复，设置通道
void NRF24L01_RX_AtuoACKPip(u8 *txbuf,u8 size,u8 pip)
{
	 NRF24L01_Write_Buf(W_ACK_PAYLOAD|pip,txbuf,size);
}

