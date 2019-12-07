/*
*   NRF24L01
* 注意:电源和地之间要加个稍大点的电容
    需要配置TXRX的长度
	2019-8-16 更新：
		增加配置24L01模块的选择，根据选择配置 RF_SETUP_DAT
*/

#ifndef __24L01_H
#define __24L01_H

#include "UHEAD.H"


//NRF24L01寄存器操作命令
#define NRF_READ_REG    0x00  //读配置寄存器,低5位为寄存器地址
#define NRF_WRITE_REG   0x20  //写配置寄存器,低5位为寄存器地址
#define RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP             0xFF  //空操作,可以用来读状态寄存器	 
//SPI(NRF24L01)寄存器地址
#define CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                              //bit4:中断MAX_RT(达到最大重发次数中断)为1使能;bit5:中断TX_DS为1使能;bit6:中断RX_DR为1使能
#define EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5

#define SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                              //bit5:数据发送完成中断;bit6:接收数据中断;
	#define	STATUS_BIT_IRT_RPTX		4		//bit4:达到最多次重发中断
	#define	STATUS_BIT_IRT_TXD		5		//bit5:数据发送完成中断
	#define	STATUS_BIT_IRT_RXD		6		//bit6:接收数据中断

#define MAX_TX  	0x10  //达到最大发送次数中断
#define TX_OK   	0x20  //TX发送完成中断
#define RX_OK   	0x40  //接收到数据中断

#define OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD              0x09  //载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define NRF_FIFO_STATUS 0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                              //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
	#define FIFO_RX_FULL	1	//接收寄存器满
	#define FIFO_TX_FULL	5	//发射寄存器满
/********************2019-8-16增加寄存器*********************************/
#define R_RX_PL_WID  	0x60  	// 查询当前这份数据的有效长度
#define	DYNPD			0X1C	// 使能动态长度
	#define	BIT_PIP0	0		// 接收数据通道0 的比特位
	#define	BIT_PIP1	1		// 接收数据通道1 的比特位
	#define	BIT_PIP2	2		// 接收数据通道2 的比特位
	#define	BIT_PIP3	3		// 接收数据通道3 的比特位
	#define	BIT_PIP4	4		// 接收数据通道4 的比特位
	#define	BIT_PIP5	5		// 接收数据通道5 的比特位
#define	FEATURE			0x1d	// 特征寄存器
	#define	FEATURE_BIT_EN_DPL		2		//使能动态载荷长度
	#define	FEATURE_BIT_EN_ACK_PAY	1		//使能有效载荷应答
	#define	FEATURE_BIT_EN_DYN_ACK	0		//使能 W_TX_PAYLOAD_NOACK 命令

#define	W_ACK_PAYLOAD 	0xA8	// 设置自动回复的通道号

//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH    5   	//5字节的地址宽度
#define RX_ADR_WIDTH    5   	//5字节的地址宽度
#define TX_PLOAD_WIDTH  32  	//6字节的用户数据宽度
#define RX_PLOAD_WIDTH  32  	//6字节的用户数据宽度

/********************2019-8-16增加功能*********************************/
#define USE_NRF_SI24

#ifdef  USE_NRF_MINI
#define RF_SETUP_DAT    0X0F    //设置发射速率为2MHZ，发射功率为最大值0dB
#endif

#ifdef  USE_NRF_SI24
#define RF_SETUP_DAT    0x0e	//设置发射速率为2MHZ，发射功率为最大值0dB
#endif

//射频数据率-SI24
#define	RF_DR_1M	0	
#define	RF_DR_2M	0x01
#define	RF_DR_250K	0x02

//射频发送功率-SI24
#define	RF_PWR_7dBm		0x07
#define	RF_PWR_4dBm		0x06
#define	RF_PWR_3dBm		0x05
#define	RF_PWR_1dBm		0x04
#define	RF_PWR_0dBm		0x03
#define	RF_PWR_sub_4dBm		0x02
#define	RF_PWR_sub_6dBm		0x01
#define	RF_PWR_sub_12dBm	0x00

#define	VALUE_RF_SETUP(dr,pwr)	(pwr|(((dr&0x01)<<3)|(((dr>>1)<<5))))

/*以下宏定义为了移植方便而定义*/		


//#define DMA_SPI

//模块版引脚定义
#define NRF24L01_CE_PIN         GPIOD,GPIO_Pin_3        //RX/TX模式选择端
#define NRF24L01_IRQ_PIN        GPIOD,GPIO_Pin_2        //可屏蔽中断端
#define NRF24L01_CSN_PIN        GPIOC,GPIO_Pin_5        //SPI片选端
#define MOSI_PIN                GPIOA,GPIO_Pin_3        //SPI主机输出从机输入端
#define MISO_PIN                GPIOA,GPIO_Pin_2        //SPI主机输出从机输出端
#define SCLK_PIN                GPIOC,GPIO_Pin_6        //SPI时钟端




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

#define REPEAT_TIME     15      //重发次数
#define	REPEAT_DELAY	15		//重复间隔，单位250uS

extern u8 RF_CH_HZ ; 
extern u8  ADDRESS1[TX_ADR_WIDTH]; //发送地址
extern u8  ADDRESS2[RX_ADR_WIDTH]; 
extern u8* address;
/*函数*/
void NRF24L01_GPIO_Init(void);					//初始化
u8 Init_NRF24L01(u8 pip,u8 rf_ch);             //初始化
void NRF24L01_RX_Mode(void);					//配置为接收模式
void NRF24L01_TX_Mode(void);					//配置为发送模式
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 u8s);                //写数据区
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 u8s);	                //读数据区		  
u8 NRF24L01_Read_Reg(u8 reg);							//读寄存器
u8 NRF24L01_Write_Reg(u8 reg, u8 value);		        //写寄存器
u8 NRF24L01_Check(void);								//检查24L01是否存在
u8 NRF24L01_TxPacket(u8 *txbuf,u8 size);				//发送一个包的数据
u8 NRF24L01_RxPacket(u8 *rxbuf,u8* len);				//接收一个包的数据
void NRF24L01_PWR(u8 state);                            //1打开0关闭电源
void FreeNRFGPIO();
//设置频率
void NRF24L01_SetRXHZ(u8 hz);
void NRF24L01_SetTXHZ(u8 hz);

/********************2019-8-16增加函数*********************************/
//获取接收到的长度信息
u8 NRF24L01_GetRXLen(void);
void NRF24L01_RX_AtuoACKPip(u8 *txbuf,u8 size,u8 pip);// RX ACK 自动回复，设置通道

/********************2019-8-18增加函数*********************************/

u8 NRF24L01_GetStatus(void);	// 获取状态status

/********************2019年9月8日增加函数*********************************/

void CreatNewAddr(u8* ChipID,u8* newAddr);				// 用ID号生产新的收发地址
void Get_ChipID(u8 *ChipID);
/********************2019年10月4日增加函数*********************************/
void NRF24L01_ResetAddr(u8* add);	//重设地址
void NRF24L01_SetRF_SETUP(u8 dr,u8 pwr);//设置射频数据率，发射功率
/********************2019年10月6日增加函数*********************************/
void NRF_CreatNewAddr(u8* addr);//依据唯一ID，产生一个新地址
/********************2019年10月30日增加函数*********************************/
void NRF24L01_GPIO_Lowpower(void);
/********************2019年11月16日增加函数*********************************/
void NRF24L01_RESUSE_TX();	// 重发上一包数据
void NRF24L01_ClearFIFO(void);

#endif













