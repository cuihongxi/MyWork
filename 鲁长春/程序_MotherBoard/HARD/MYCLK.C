#include "MyCLK.H"

//    关闭所有外设时钟
void CLOSE_CLK_ALL(void)
{
    CLK->PCKENR1 = 0;
    CLK->PCKENR2 = 0;
}


//    打开外设时钟1

void OPEN_CLK1(unsigned char Clk_Peripheral1)
{
    assert_param(IS_Peripheral1_SOURCE(Clk_Peripheral1));

    CLK->PCKENR1 |= Clk_Peripheral1;
}

//打开外设时钟2

void OPEN_CLK2(unsigned char Clk_Peripheral2)
{
    assert_param(IS_Peripheral2_SOURCE(Clk_Peripheral2));
    CLK->PCKENR2 |= Clk_Peripheral2;
}

//  关闭外设时钟1

void CLOSE_CLK1(unsigned char Clk_Peripheral1)
{
    assert_param(IS_Peripheral1_SOURCE(Clk_Peripheral1));

    CLK->PCKENR1 &= ~Clk_Peripheral1;
}

//关闭外设时钟2

void CLOSE_CLK2(unsigned char Clk_Peripheral2)
{
   assert_param(IS_Peripheral2_SOURCE(Clk_Peripheral2));
    CLK->PCKENR2 &= ~Clk_Peripheral2;
}

//HSI内部时钟校正值

void MyCLK_AdjustHSICalibrationValue(unsigned char CLK_HSICalibrationValue)
{

  CLK->HSITRIMR = (CLK->HSITRIMR &0x0f)|CLK_HSICalibrationValue;
}

/**配置HSI和CPU分频器
  * @brief  Configures the HSI and CPU clock dividers.
  * @param   ClockPrescaler Specifies the HSI or CPU clock divider to apply.
  * @retval None
  */
void MyCLK_SYSCLKConfig(CLK_CKDIVR_HIS_CPU CLK_Prescaler)
{

  if(CLK_Prescaler > 0x0f)    //判断是HSI还是CPU分频  
  {
   
    CLK->CKDIVR  &= 0XE7;
    CLK->CKDIVR |= ((u8)CLK_Prescaler - 0X10);
  }else
  {
    CLK->CKDIVR &= 0xF8;
    
    CLK->CKDIVR |= CLK_Prescaler;
  }
  
}

//内部16M晶振
void CLK_INIT(CLK_CKDIVR_HIS_CPU HIS_clk,CLK_CKDIVR_HIS_CPU CUP_clk)
{
   MyCLK_SYSCLKConfig(HIS_clk); //配置HSI或者CPU的分频器
   MyCLK_SYSCLKConfig(CUP_clk); //配置HSI或者CPU的分频器
}

//切换LSI时钟源
void CLK_Change2LSI()
{
	CLK_HSICmd (DISABLE); 
	CLK_LSICmd (ENABLE);    							//使能LSI
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);		//LSI为系统时钟
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  //RTC时钟源LSI，
    while(CLK_GetFlagStatus(CLK_FLAG_LSIRDY)==RESET);	//等待LSI准备就绪
    CLK_SYSCLKSourceSwitchCmd (ENABLE);					//使能切换
}

//切换HSI时钟源
void CLK_Change2HSI()
{
	CLK_LSICmd (DISABLE); 
	CLK_HSICmd (ENABLE);    							//使能HSI
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);		//HSI为系统时钟
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)==RESET);	//等待HSI准备就绪
    CLK_SYSCLKSourceSwitchCmd (ENABLE);					//使能切换
}

