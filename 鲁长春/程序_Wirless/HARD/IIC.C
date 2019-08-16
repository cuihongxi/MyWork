#include "IIC.H"

/********************************************************************************
*Author : explorersoftware.taobao.com 
*Time   :  
*Explain:
*
********************************************************************************/

void IIC_Init()
{
    SCL_OUT;
    SDA_OUT;    
}

//起始信号
void IIC_Start(void)
{  
  JUGE_BUSY();
  SDA_OUT;
  SDA_PIN_LOW;
  SDA_PIN_HIGH;                      
  SCL_PIN_HIGH;
  DELAY_2US;
  SDA_PIN_LOW;
  DELAY_2US;
  SCL_PIN_LOW;  
}


//I2C从机应答
 answerType IIC_Response(void)         
{
   u8 i=0;
   SDA_IN;
   SDA_PIN_HIGH; 
   SCL_PIN_LOW;
   while(SDAPIN_READ_IN != 0)
   {
        i++;
        if(i==250) 
        {
#if  DEBUG_IIC > 0
          USART1_SendSTR("no answer\r\n");
          
#endif

            return N_ANSWER;
        }
   }
#if  DEBUG_IIC > 0
   USART1_SendSTR("answer!!!!\r\n");
#endif   

   DELAY_2US;
   SCL_PIN_HIGH;   
   DELAY_2US;
   SCL_PIN_LOW;
   return ANSWER;
}

//I2C主机应答
void IIC_MasterResponse(answerType answer)
{    
    SDA_IN;
    SCL_PIN_LOW;  
   JUGE_BUSY();
    SDA_OUT;
    if(answer)
    SDA_PIN_LOW;
    else SDA_PIN_HIGH; 
    DELAY_2US;
    SCL_PIN_HIGH;
    DELAY_2US;
    SCL_PIN_LOW;    
    SDA_PIN_HIGH;    
}

//停止
void IIC_Stop(void)
{  
  SDA_OUT;
  SCL_PIN_LOW;
  JUGE_BUSY();
  SDA_PIN_LOW;  
  SCL_PIN_HIGH;
  DELAY_2US;  
  SDA_PIN_HIGH;    
}

//写数据
void IIC_WriteByte(u8 byte)
{
    u8 i;    
    SDA_OUT;
    SCL_PIN_LOW;
   JUGE_BUSY();
    for(i=0;i<8;i++)
    {                   
        if(byte & 0x80) SDA_PIN_HIGH; 
        else SDA_PIN_LOW;        
        SCL_PIN_HIGH;
        byte <<= 1;
        DELAY_2US;
        SCL_PIN_LOW;
        DELAY_2US;
    }
    SDA_PIN_HIGH;
}


//读数据
u8 IIC_ReadByte(void)
{
    u8 i,dat = 0;
    SDA_IN;
    for(i=0;i<8;i++)
    {        
        SCL_PIN_LOW;
        DELAY_2US;
        SCL_PIN_HIGH;
        DELAY_2US;
        dat <<= 1; 
        if(SDAPIN_READ_IN) dat ++;
        DELAY_2US;
                       
    }
    return dat;
}
//单字节写入
//base_addr器件地址
void IIC_WriteREGByte(u16 base_addr,u8 reg)
{
    SCL_PIN_LOW;  
    IIC_Start();  
    //传送地址+w
    IIC_WriteByte(base_addr);  
    IIC_Response() ;
    //要写入的地址
    IIC_WriteByte(reg);
    IIC_Response() ;
}
