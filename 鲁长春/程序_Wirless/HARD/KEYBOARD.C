#include "keyboard.h"
#include "stm8l15x_exti.h"
#include "24l01.h"
u8      	flag_exti 		= 0 ;
u8 		DM_time 		= 0;


extern		u32			systime;
extern 		u8 			keyval ;

//按键GIPO横向IO模式设定
void GPIO_Heng_MOED_SET(GPIO_Mode_TypeDef GPIO_MODE)
{
    GPIO_Init(Heng_0,GPIO_MODE);
    GPIO_Init(Heng_1,GPIO_MODE);
    GPIO_Init(Heng_2,GPIO_MODE);
}
//按键GIPO列向IO模式设定
void GPIO_Lie_MOED_SET(GPIO_Mode_TypeDef GPIO_MODE)
{
    GPIO_Init(Lie_0,GPIO_MODE);
    GPIO_Init(Lie_1,GPIO_MODE);
    GPIO_Init(Lie_2,GPIO_MODE);
}

//给端口赋值
void    Set_Valu(u8 dat)
{
      if((dat&0x01) == 0) GPIO_ResetBits(Lie_0);              
      else  GPIO_SetBits(Lie_0);
      if((dat&0x02) == 0) GPIO_ResetBits(Lie_1);
      else  GPIO_SetBits(Lie_1);
      if((dat&0x04) == 0) GPIO_ResetBits(Lie_2);
      else  GPIO_SetBits(Lie_2);

        if((dat&0x10) == 0) GPIO_ResetBits(Heng_0);
      else  GPIO_SetBits(Heng_0);
        if((dat&0x20) == 0) GPIO_ResetBits(Heng_1);
      else  GPIO_SetBits(Heng_1);
        if((dat&0x40) == 0) GPIO_ResetBits(Heng_2);
      else  GPIO_SetBits(Heng_2);

}

//读端口数据
u8    Read_Valu()
{
    u8  dat = 0;
  
    if(GPIO_ReadInputDataBit(Lie_0) != 0) dat|=0x01;
    if(GPIO_ReadInputDataBit(Lie_1) != 0) dat|=0x02;
    if(GPIO_ReadInputDataBit(Lie_2) != 0) dat|=0x04;

    if(GPIO_ReadInputDataBit(Heng_0)  != 0) dat|=0x10;
    if(GPIO_ReadInputDataBit(Heng_1)  != 0) dat|=0x20;
    if(GPIO_ReadInputDataBit(Heng_2)  != 0) dat|=0x40;
    return dat;
}

//检测哪个按键拿下，返回键值
u8  Keyscan()
{
    u8  KeyPort=0; 
    GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
    GPIO_Lie_MOED_SET(GPIO_MODE_IN);    //列读数
    if(Read_Valu() != 0x07)
    {      
       // delay_ms(5);
        KeyPort = Read_Valu();
        if(KeyPort!= 0x07)
        {      
            switch (KeyPort)
            {
                case 0X06:      
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();
                                switch (KeyPort)
                                {								
                                    case 0x30: return(KEY_VAL_MOTY);	 
                                        break;
                                    case 0x50: return(KEY_VAL_I100);
                                        break;
                                    case 0x60: return(KEY_VAL_Y30);
                                        break;

                                };
                    break;
                case 0x05:     
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();

                                switch (KeyPort)
                                {
                                     case 0x30: return(KEY_VAL_MOTZ);
                                        break;
                                    case 0x50: return(KEY_VAL_I60);
                                        break;
                                    case 0x60: return(KEY_VAL_POW_CA);
                                        break;
                                };
                    break;
                case 0x03:     
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();
                                
                                switch (KeyPort)
                                {
                                    case 0x30: DM_time = 1;return (KEY_VAL_DUIMA);
                                        break;
                                    case 0x50: return(KEY_VAL_I30);	
                                        break;
                                    case 0x60: return(KEY_VAL_AM);
                                        break;
                                };
                    break;

            }
            
        }
    }
	
    return(KEY_VAL_NULL);
    
}
//松手程序
void Key_ScanLeave()
{

    GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
    GPIO_Lie_MOED_SET(GPIO_MODE_IN);    //列读数    
							
    if(Read_Valu() == 0x07)
    {       
	debug("key null\r\n");
        
        GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
        GPIO_Lie_MOED_SET(GPIO_MODE_IT);    //列读数 
	if(DM_time)
	{
	   // debug(" systime = %lu",(u32)systime);
	    if(systime < 20000)
	    {
		debug(" :DM \r\n");
	    }
	    if(systime > 100000)debug(" :clear DM \r\n");
	    systime = 0;
	    DM_time = 0;
	    keyval = KEY_VAL_NULL;
	}
	
	flag_exti = 0;
	
    }
}

void Key_GPIO_Init()
{
      GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
      GPIO_Lie_MOED_SET(GPIO_MODE_IT);    //列读数
      flag_exti = 0;
	  disableInterrupts();
	  EXTI_SetPinSensitivity(EXTI_Pin_0,EXTI_Trigger_Falling);
	  EXTI_SetPinSensitivity(EXTI_Pin_3,EXTI_Trigger_Falling);
      enableInterrupts();                                                 //使能中断
}


//IRQ 中断服务函数 D0 A0 //NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
  if(GPIO_READ(Lie_1)== RESET || GPIO_READ(Lie_2)== RESET)
  {
   	    if(keyval ==KEY_VAL_NULL)
		{
			NRF24L01_PWR(1);
			flag_exti = 1;
			keyval = Keyscan();  
		}
  }

   EXTI_ClearITPendingBit (EXTI_IT_Pin0);
} 
 

//IRQ 中断服务函数 A3
INTERRUPT_HANDLER(EXTI3_IRQHandler,11)
{
   if(GPIO_READ(Lie_0)== RESET)  
   {
   	    if(keyval ==KEY_VAL_NULL)
		{
			NRF24L01_PWR(1);
			flag_exti = 1;  
			keyval = Keyscan();    
		}
   }
   EXTI_ClearITPendingBit (EXTI_IT_Pin3);
} 


