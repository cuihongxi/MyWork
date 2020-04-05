#include "keyboard.h"
#include "stm8l15x_exti.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"
#include "stmflash.h"
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes);
u8      	flag_exti 		= 0 ;
u8			flag_touch		= 0;
u32 		DM_time 		= 0;
u32 		AM_time 		= 0;
u32 		POW_CA_time 	= 0;
u32 		Y30_time 		= 0;
u32			I30_time		= 0;
//u8			flag_funAM		= 0;
u8			flag_funPOW_CA	= 0;
//u8			flag_I30_en		= 0;

extern		u32					systime;
extern 		u8 					keyval ;
extern 		Nrf24l01_PTXStr 	ptx;
extern 		u8 					pressKey;
extern 		u8	                flag_duima;
extern 		u8					LEDtimes;
extern 		u32					sendtime;
extern 		u8 					db ;

void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes);// 通过NRF向主板发送命令函数
void LEN_RED_Open();
void LEN_RED_Close();
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
      if((dat&0x01) == 0) GPIO_RESET(Lie_0);              
      else  GPIO_SET(Lie_0);
      if((dat&0x02) == 0) GPIO_RESET(Lie_1);
      else  GPIO_SET(Lie_1);
      if((dat&0x04) == 0) GPIO_RESET(Lie_2);
      else  GPIO_SET(Lie_2);

        if((dat&0x10) == 0) GPIO_RESET(Heng_0);
      else  GPIO_SET(Heng_0);
        if((dat&0x20) == 0) GPIO_RESET(Heng_1);
      else  GPIO_SET(Heng_1);
        if((dat&0x40) == 0) GPIO_RESET(Heng_2);
      else  GPIO_SET(Heng_2);

}

//读端口数据
u8    Read_Valu()
{
    u8  dat = 0;
  
    if(GPIO_READ(Lie_0) != 0) dat|=0x01;
    if(GPIO_READ(Lie_1) != 0) dat|=0x02;
    if(GPIO_READ(Lie_2) != 0) dat|=0x04;

    if(GPIO_READ(Heng_0)  != 0) dat|=0x10;
    if(GPIO_READ(Heng_1)  != 0) dat|=0x20;
    if(GPIO_READ(Heng_2)  != 0) dat|=0x40;
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
                                    case 0x50: debug("I100\r\n");return(KEY_VAL_I100);
                                        break;
                                    case 0x60: Y30_time = systime;debug("Y30\r\n");return(KEY_VAL_Y30);
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
                                    case 0x50: debug("I60\r\n");return(KEY_VAL_I60);
                                        break;
                                    case 0x60: POW_CA_time = systime;return(KEY_VAL_POW_CA);
                                        break;
                                };
                    break;
                case 0x03:     
                                GPIO_Heng_MOED_SET(GPIO_MODE_IN);  //横读数
                                GPIO_Lie_MOED_SET(GPIO_MODE_OUT);  //列发0
                                KeyPort = Read_Valu();
                                
                                switch (KeyPort)
                                {
                                    case 0x30: DM_time = systime;return (KEY_VAL_DUIMA);
                                        break;
                                    case 0x50: I30_time = systime;debug("I30\r\n");return(KEY_VAL_I30);	
                                        break;
                                    case 0x60: AM_time = systime;return(KEY_VAL_AM);
                                        break;
                                };
                    break;

            }
            
        }
    }
	
    return(KEY_VAL_NULL);
    
}
void Key_TouchtLeave()
{

	if(GPIO_READ(TOUCH_IO))	// 离开
	{
	  //  if((systime - sendtime)>=1000)
		{
		  GPIO_Init(TOUCH_IO,GPIO_MODE_TOUCH);
		  flag_touch = 0;
		  LEN_RED_Close();
		}
	}else				
  	if((systime - sendtime) >= 8000)			// 还在按着
	{
	  GPIO_Init(TOUCH_IO,GPIO_MODE_TOUCH);
	 // flag_touch = 0;
		NRF_SendCMD(&ptx,ADDRESS2,CMD_WAKE, MES_WAKE_UP);
	}	  
	  
}
//松手程序
void Key_ScanLeave()
{

    GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
    GPIO_Lie_MOED_SET(GPIO_MODE_IN);    //列读数    
	
	if(keyval == KEY_VAL_AM)
	{
	    if((systime - AM_time) > DELAY_TIME)
	    {
			//debug("切换AM\r\n");	
			NRF_SendCMD(&ptx,ADDRESS2,CMD_AM, MES_AM);//NRF发送AM命令
			keyval = KEY_VAL_NULL; 
	    } 
	}  
	if(keyval == KEY_VAL_POW_CA)
	{
	    if((systime - POW_CA_time) > DELAY_TIME)
	    {
			//flag_funPOW_CA= ~flag_funPOW_CA;
//			debug("POW_CA\r\n");
//			//NRF?????????è?¨
//
//			db ^= 6;
//			LEDtimes = 2*LEDPOWTIM;
//			NRF24L01_SetRF_SETUP(RF_DR_250K,db);
//			keyval = KEY_VAL_NULL;
		  NRF_SendCMD(&ptx,ADDRESS2,CMD_AM, MES_AM);//NRF发送AM命令
	    }
	}
	
	if(keyval == KEY_VAL_Y30)
	{
	    if((systime - Y30_time) > DELAY_TIME)
	    {
			//debug("Y30取消\r\n");
			NRF_SendCMD(&ptx,ADDRESS2,CMD_Y30, MES_Y30_CLEAR); 
			keyval = KEY_VAL_NULL;
	    }
	} 
//	if(keyval == KEY_VAL_I30)
//	{
//	    if((systime - I30_time) > DELAY_TIME)
//	    {
//			//debug("开窗报警\r\n");
//			NRF_SendCMD(&ptx,ADDRESS3,CMD_I30, MES_I30_ALARM); 
//			keyval = KEY_VAL_NULL;
//	    }
//	    
//	} 	
    if(Read_Valu() == 0x07)	//无按键按下
    {       

        GPIO_Heng_MOED_SET(GPIO_MODE_OUT);  //横发 0
        GPIO_Lie_MOED_SET(GPIO_MODE_IT);    //列读数 
		if(keyval == KEY_VAL_DUIMA)
		{
		  // debug(" (systime - DM_time) = %lu",(systime - DM_time));
			if((systime - DM_time)> 500 && (systime - DM_time) < 8000)
			{
			  		flag_duima = 1;
					//debug("DM 模式\r\n");
			}
//			if((systime - DM_time) > 6000)
//				{
//				  ClearDM();
//				}
			DM_time = 0;
			pressKey = 1;
		}
		if(keyval == KEY_VAL_AM)
		{
			if((systime - AM_time) < 3000)
				NRF_SendCMD(&ptx,ADDRESS2,CMD_AM, MES_AM_CHECK);//NRF发送AM命令	
		}
		if(keyval == KEY_VAL_Y30)
		{
			if((systime - Y30_time) < 3000)
			{
				NRF_SendCMD(&ptx,ADDRESS2,CMD_Y30, MES_Y30_CHECK); 
				//debug("MES_Y30_CHECK\r\n");
			}
				
		}
		flag_exti = 0;
		//GPIO_SET(Z_LED);
		keyval = KEY_VAL_NULL;
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


//IRQ 中断服务函数 D0 A0
INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
  if(GPIO_READ(Lie_1)== RESET || GPIO_READ(Lie_2)== RESET)
  {
   	    if(keyval ==KEY_VAL_NULL)
		{
			flag_exti = 1;
			keyval = Keyscan();  
         //   GPIO_RESET(Z_LED);
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
			flag_exti = 1;  
			keyval = Keyscan();
           // GPIO_RESET(Z_LED);
		}
   }
   EXTI_ClearITPendingBit (EXTI_IT_Pin3);

} 

INTERRUPT_HANDLER(EXTI5_IRQHandler,13)
{
  if(flag_touch == 0)
   {
   	    if(GPIO_READ(TOUCH_IO) == 0) 
		{ 
		     flag_touch = 1; 
			 LEDtimes = LEDPOWTIM*2;	
			 NRF_SendCMD(&ptx,ADDRESS2,CMD_WAKE, MES_WAKE_UP);
			 
			 GPIO_Init(TOUCH_IO,GPIO_Mode_In_PU_No_IT);
			 LEN_RED_Open();
		}
   } 
 	EXTI_ClearITPendingBit (EXTI_IT_Pin5);
}
