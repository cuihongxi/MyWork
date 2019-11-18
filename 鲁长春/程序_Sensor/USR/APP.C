#include "APP.H"

u8 key_val              = 0;            //键值
u8 flag_THRstart        = 1;            //温湿度检测开始标志
u8 flag_THRstarting     = 1;            //温湿度检测进行中标志
u8 counter_THR          = 0;            //温湿度检测计数器
s16 max_T[2]           = {0};          //最高温度
s16 max_H[2]           = {0};          //最高湿度
s16 min_T[2]           = {0};          //最低温度
s16 min_H[2]           = {0};          //最低湿度
u32 counter_timer       = 0;            //小时计时器  
u32 counter_enter       = 0;
u8      flag_enter_match = 0;   //进入匹配标志
u8 flag_lowhightime     = 0;            //小时更新标志
u8 flag_sharp           = 0;            //闪烁标志
u8 flag_sharpstart      = 0;            //闪烁开始
u8 flag_sharping        = 0;            //闪烁已经进行
u8 mod             = MODE_ALLON|MODE_ON;     //模式
u8      hour            = 0;            //on小时
u8      flag_nrf        = 0;            //当该标志位为1时NRF发送数据
u8      flag_onffhour   = 1;            //刷新开关小时标志
u8      flag_mode       = 1;            //模式显示刷新标志
u8      flag_wake       = 1;            //睡眠\唤醒标识  0睡眠1唤醒
u16     time_wakeup     = 3;           //唤醒10秒间隔
u16      count_sleep     = 0;            //睡眠计数器
u8      Tem_HR_Check_i = 0;
u8      on_off_switch = 0;              //开关切换标志
extern const int fuhao0[] ;
extern const int dat_100_1[];
extern const int dat_100_0[];
extern const int dat_12x16_100_1[];
extern const int dat_12x16_100_0[];

// 参数初始化
void DATA_Init()
{
    key_val              = 0;            //键值
    flag_THRstart        = 0;            //温湿度检测开始标志
    flag_THRstarting     = 0;            //温湿度检测进行中标志
    counter_THR          = 0;            //温湿度检测计数器
    counter_timer        = 0;            //小时计时器  
    flag_lowhightime     = 0;            //小时更新标志
    flag_sharp           = 0;            //闪烁标志
    flag_sharpstart      = 0;            //闪烁开始
    flag_sharping        = 0;            //闪烁已经进行
    flag_nrf        = 0;            //当该标志位为1时NRF发送数据
    flag_onffhour   = 1;            //刷新开关小时标志
    flag_mode       = 1;            //模式显示刷新标志 
    count_sleep = 0;
}
//NRF发送数据
void SendNrfDate(u8 mode,u8 hour)
{
    u8 dat[6]={0};                  //射频发送BUF
   // u8 i = 0;
    //u8 hz = RF_CH_HZ;          //临时保存
    dat[1] = (u8)mode|hour;
    dat[0] = HEAD_MESSAGE;   //开头密码
    NRF24L01_TxPacket(dat);

}


//去掉24小时前的数据
void Rolloarray(s16* array)
{
  array[1] = array[0];
}

//找出最大最小值
s16 FindMin(s16* array)
{
    s16 mindat = array[0];
    if(array[1] == 0) return mindat;
    else if(mindat>array[1]) mindat = array[1];      
    return mindat;
}

s16 FindMax(s16* array)
{
    s16 maxdat = array[0];
    if(array[1] == 0) return maxdat;
    else if(maxdat < array[1]) maxdat = array[1]; 
    return maxdat;
}

//TIM2初始化
void TIM2_INIT()
{
    enableInterrupts();                                                 //使能全局中断
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);  //打开时钟
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_Prescaler_128,TIM2_CounterMode_Up,62500);     //8us,500ms进入一次
    TIM2_ARRPreloadConfig(ENABLE);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
    TIM2_ClearITPendingBit(TIM2_IT_Update); 
    TIM2_Cmd(ENABLE);
}

//温湿度检测 , 检测一个T和H返回0 ,否则返回1
u8 Tem_HR_Check()
{
    static u8 flag_TorH = SH20_SLAVE_TEST_T; 
    static u8 Tem_HR_first = 0;
    s16 dat_TRH;
    if(flag_THRstarting) 
    {
        if(flag_THRstart)
        {
            SH20_StartMeasure(flag_TorH);  
            flag_THRstart = 0;
            if(flag_TorH == SH20_SLAVE_TEST_RH) flag_TorH = SH20_SLAVE_TEST_T;
            else flag_TorH = SH20_SLAVE_TEST_RH;
        }else if(SH20_ReadMeasure(&dat_TRH)== ANSWER)     
        {
          if(flag_TorH == SH20_SLAVE_TEST_T)                    //在开始这个值会被切换成下个状态值
          {
            //有效性判断
            if(dat_TRH>99)      dat_TRH = 99;
            else if(dat_TRH<1)  dat_TRH = 1;
            
            LCD_Show_Data(&font_24x24_2,LCD_HR_POS,dat_TRH);      //显示湿度
            if(dat_TRH > max_H[0]) max_H[0] = dat_TRH;
            if(min_H[0] == 0 || dat_TRH < min_H[0])min_H[0] = dat_TRH;
          }
          else 
          {
           // dat_TRH = -4;
            //判断有没有大于100
            if(dat_TRH >=100)                 
                COG12864_Display(dat_100_1,12,24,0,5);          //写1                        
            else COG12864_Display(fuhao0,12,24,0,5);            //清空
            


            
            //有效性判断
            if(dat_TRH< -4) dat_TRH = -4;               
            else if(dat_TRH>158) dat_TRH = 158;         
            LCD_Show_Data(&font_24x24,LCD_TP_POS,dat_TRH);      //显示温度    
            if(Tem_HR_first == 0)
            {
                Tem_HR_first =1;
                //dat_TRH = -2;
                max_T[0] = min_T[0] = dat_TRH;
            }
            if(dat_TRH > max_T[0]) max_T[0] = dat_TRH;
            if(min_T[0]==0 || dat_TRH < min_T[0]) min_T[0] = dat_TRH;                       
          }
        flag_THRstarting = 0;
          if(flag_wake == 0)
          {
                Tem_HR_Check_i ++;              
                if(Tem_HR_Check_i<2)
                {
                    
                    flag_THRstart = 1; 
                    flag_THRstarting = 1;                
                }else 
                {
                    Tem_HR_Check_i=0;
                    return 0;
                }
                
          }
        }
        
        
    }
    return 1;
}

//清除模式位置的数字
void Clear_Lcd()
{
    CUIGUI_SetFont(&font_12x12);
    LCD_ShowStr(LCD_TIMEON_POS,"  ");    
    LCD_ShowStr(LCD_TIMEOFF_POS,"  ");
}

//显示模式位置的数字
void Show_Tonoff(u8 dat)
{
    LCD_Show_Data(&font_12x12,LCD_TIMEON_POS,dat);
    LCD_Show_Data(&font_12x12,LCD_TIMEOFF_POS,24-dat);
}

//睡眠设置
void Sleep_Fun()
{
    NRF24L01_GPIO_Init();
    COG12864_GPIO_Init();
    Show_Tonoff(hour);   
    FreeLCDGPIO();    
    Key_GPIO_Init();
    GPIO_Init(SCL_PIN,GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(SDA_PIN,GPIO_Mode_Out_PP_High_Slow);    
    NRF24L01_PWR(0);
    FreeNRFGPIO();
    

}

//使用完的端口释放,下拉输出低


void FreeLCDGPIO()
{ 
//    GPIO_Init(COG12864_CS0_PIN,GPIO_Mode_Out_PP_High_Slow);
//    GPIO_Init(COG12864_RST_PIN,GPIO_Mode_Out_PP_High_Slow);
//    GPIO_Init(COG12864_SDA_PIN,GPIO_Mode_Out_PP_High_Slow);
//    GPIO_Init(COG12864_RS_PIN,GPIO_Mode_Out_OD_Low_Slow);
//    GPIO_Init(COG12864_CLK_PIN,GPIO_Mode_Out_OD_Low_Slow);          
}

//定时唤醒检测温度函数
void Wake_CheckHT()
{
    //时钟初始化                                           
////    CLK->CKDIVR  &= 0XE7;   
////    CLK->CKDIVR &= 0xF8;    
////    CLK->ICKCR |= CLK_ICKCR_HSION;
    
    flag_THRstart = 1; 
    flag_THRstarting = 1;  
   Tem_HR_Check_i = 0;
//#if  SHT20 > 0
   // SH20_Init();
    while(Tem_HR_Check())
    {
        if(flag_exti)return;//如果按键中断就跳出
    }
//#endif

    Show_MaxMin();

}

//显示最大最小值
void Show_MaxMin()
{
    s16 dat = FindMin(min_T);
    //显示温度最小值
    //dat = 160;
    if(dat >=100)                 
    {
        COG12864_Display(dat_12x16_100_1,12,16,45,6);
    }else
    {
        COG12864_Display(dat_12x16_100_0,12,16,45,6);
    }
    LCD_Show_Data(&font_16x16_2,LCD_MIN_TP_POS,dat);
    //显示温度最大值
    dat = FindMax(max_T);
    
    //dat = 162;
    if(dat >=100)                 
    {
        COG12864_Display(dat_12x16_100_1,12,16,83,6);
    }else
    {
        COG12864_Display(dat_12x16_100_0,12,16,83,6);
    }
    LCD_Show_Data(&font_16x16_2,LCD_MAX_TP_POS,dat);
    
     //显示湿度
    LCD_Show_Data(&font_16x16,LCD_MIN_HR_POS,FindMin(min_H));
    LCD_Show_Data(&font_16x16,LCD_MAX_HR_POS,FindMax(max_H));
    
    
}