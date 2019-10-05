#include "PWM.H"


//******************************************************************************        
      
//introduce:        PWM初始化                  
                   
//******************************************************************************  

void PWM_Init(void)
{
  	//PB0通道IO配置为输出
  	GPIO_Init(PWM_PIN, GPIO_Mode_Out_PP_High_Fast); 
	PWM_BASE_INIT(PWM_TIME,PWM_PRESCALER);
  	PWM_Status(PWM_OFF);
	
}

//******************************************************************************           
//introduce:        PWM设置占空比    
//parameter:        dutyCycle: 0~100为0%~100%                                     
//changetime:       2016.12.01                        
//******************************************************************************  
void PWM_SetDutyCycle(u8 dutyCycle)
{
  dutyCycle %= 101;
  PWM_SETCMP(((PWM_PERIOD*CRYFRE)/PWM_PRESCALER)*dutyCycle/100); 
  
}
 
//******************************************************************************        
    
//introduce:        PWM开关    
                  
//******************************************************************************  
void PWM_Status(u8 status)
{
  //打开PWM
  if(status == PWM_ON)
  {
	PWMOUTPUT(PWM_TIME,ENABLE);
   	GPIO_RESET(PWM_PIN);       
  }
  //关闭PWM
  else
  {
	PWMOUTPUT(PWM_TIME,DISABLE);
    	GPIO_SET(PWM_PIN);     
  }
}


