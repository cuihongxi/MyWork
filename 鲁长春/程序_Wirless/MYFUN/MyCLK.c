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