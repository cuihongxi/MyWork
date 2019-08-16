#include "SH20.H"


//初始化GPIO
void SH20_Init()
{
    IIC_Init(); 
    //SH20_DELAY15MS;
    SH20_Rest();
    
}

//停止SH20测量
void SH20_Stop()
{
     IIC_Stop(); 
}

//配置成非主机,启动一次测量
//开启定时器,等待100MS以上去读测量结果
void SH20_StartMeasure(u8 comd)
{
    IIC_Init();
    IIC_WriteREGByte(BASE_ADDR,comd);
}

//非主机模式下读
answerType SH20_SlaveMeasure(u8 comd,s16* datbuf)
{
    u8 crc_dat = 0;
    IIC_Init();
    IIC_WriteREGByte(BASE_ADDR,comd);
    IIC_Start();
    //传送地址+r
    IIC_WriteByte(BASE_ADDR+1);
    IIC_Response();
    SCL_PIN_HIGH;
    
    SH20_DUGE_BUSY(); //等待释放CLK
    *datbuf = IIC_ReadByte();        
    IIC_MasterResponse(ANSWER);
    *datbuf = (*datbuf<<8)|IIC_ReadByte(); 
    IIC_MasterResponse(ANSWER);

    crc_dat = IIC_ReadByte();       //读CRC
    IIC_MasterResponse(N_ANSWER);
    IIC_Stop();   
    if(cal_crc_table((unsigned char *)datbuf,2) != crc_dat)
    {
     return N_ANSWER;              //校验错误返回
    }
    //处理数据
   if(*datbuf & 0x0002)         //湿度
   {
        *datbuf = (s16)((((u32)(*datbuf&0xfffc)*125)>>16) - 6);
        
   }else                        //温度
   {
        *datbuf = (s16)(((u32)((175.72*(*datbuf&0xfffc)))>>16) - 46.85);
   }
      
   return ANSWER; 
}

//启动测量之后100MS以上读取测量结果,并停止SH20
//无应答返回 N_ANSWER,应答反之
//datbuf 保存读取的数据--温度值或湿度值
answerType SH20_ReadMeasure(s16* datbuf)
{
    u8 crc_dat = 0;
    IIC_Init();
    IIC_Start();
    //传送地址+r
    IIC_WriteByte(BASE_ADDR+1);
    //等待应答
   if(IIC_Response() == N_ANSWER) //无应答,继续测量,跳过读取
   {
        return N_ANSWER;              //无应答
   }else
   {
        *datbuf = IIC_ReadByte();        
        IIC_MasterResponse(ANSWER);
        *datbuf = (*datbuf<<8)|IIC_ReadByte(); 
        IIC_MasterResponse(ANSWER);
        
        crc_dat = IIC_ReadByte();       //读CRC
        IIC_MasterResponse(N_ANSWER);
        IIC_Stop();   
        if(cal_crc_table((unsigned char *)datbuf,2) != crc_dat)
        {
             return N_ANSWER;              //校验错误返回
        }
        
         //处理数据
       if(*datbuf & 0x0002)         //湿度
       {
            *datbuf = (s16)((float)(((u32)(*datbuf&0xfffc)*125)>>16) - 6+ 0.5);
            
       }else                        //温度
       {
            *datbuf = ((s16)(((float)((u32)(175.72*(*datbuf&0xfffc))>>16) - 46.85)*1.8+ 0.5) + 32 );//华氏度(℉)=32+摄氏度(℃)×1.8 
            
       }
   }

   
   return ANSWER;      
}

//软复位
void SH20_Rest()
{
    IIC_WriteREGByte(BASE_ADDR,SH20_SOFT_REST);
    IIC_Stop(); 
    SH20_DELAY15MS;
}

//SH20低功耗
void SH20_LowPower(void)
{
    SCL_PIN_HIGH;
    SDA_PIN_HIGH;
    
}
