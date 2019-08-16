#include "keyboard.h"
#include "stm8l15x_exti.h"


u8      flag_exti = 0 ;


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
    GPIO_Init(Lie_3,GPIO_MODE);
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
        if((dat&0x08) == 0) GPIO_ResetBits(Lie_3);
      else  GPIO_SetBits(Lie_3);
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
    if(GPIO_ReadInputDataBit(Lie_3) != 0) dat|=0x08;
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
    
    if((Read_Valu() != 0x0F))
    {      
        delay_ms(10);
        KeyPort = Read_Valu();
        if((KeyPort!= 0x0F))
        {             
            switch (KeyPort)
            {
                case 0X0E:      
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();
                                switch (KeyPort)
                                {
                                    case 0x30: return(KEY_VAL_TIMER);
                                        break;
                                    case 0x50: return(KEY_VAL_MOD1);
                                        break;
                                    case 0x60: return(KEY_VAL_ALLOFF);
                                        break;

                                };
                    break;
                case 0x0D:     
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();

                                switch (KeyPort)
                                {
                                     case 0x30: return(22);
                                        break;
                                    case 0x50: return(KEY_VAL_MOD2);
                                        break;
                                    case 0x60: return(KEY_VAL_VEG);
                                        break;
                                };
                    break;
                case 0x0B:     
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();
                                
                                switch (KeyPort)
                                {
                                    case 0x30: return(KEY_VAL_ADD);
                                        break;
                                    case 0x50: return(KEY_VAL_MOD3);
                                        break;
                                    case 0x60: return(KEY_VAL_BLO);
                                        break;
                                };
                    break;
               case 0x07:     
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();
                               
                                switch (KeyPort)
                                {
                                    case 0x30: return(KEY_VAL_SUB);
                                        break;
                                    case 0x50: return(KEY_VAL_MOD4);
                                        break;
                                    case 0x60: return(KEY_VAL_ALLON);
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
    if(Read_Valu() == 0x0F)
    {       
        flag_exti = 0;

        GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
        GPIO_Lie_MOED_SET(GPIO_MODE_IT);    //列读数 
    }
}

void Key_GPIO_Init()
{
      GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
      GPIO_Lie_MOED_SET(GPIO_MODE_IT);    //列读数
      flag_exti = 0;
      EXTI_SelectPort(EXTI_Port_B);
      EXTI_SetPortSensitivity(EXTI_Port_B,EXTI_Trigger_Falling);//设置端口敏感性
      EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB,ENABLE);    
      enableInterrupts();                                                 //使能中断
}


INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
//    if(key_val ==KEY_VAL_NULL)
//    {
//        flag_exti = 1;  
//        key_val = Keyscan();    
//    }
//         count_sleep = 0;

        EXTI_ClearITPendingBit (EXTI_IT_PortB);  
       
}

