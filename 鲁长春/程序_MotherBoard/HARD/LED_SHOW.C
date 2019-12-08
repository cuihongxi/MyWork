#include "UHEAD.H"
#include "LED_SHOW.H"

void BeepStart();
void BeepStop();

//双色LED初始化
void LED_GPIO_Init()
{
    GPIO_Init(LED_GREEN,GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(LED_RED,GPIO_Mode_Out_PP_High_Slow);
}

void LEN_RED_Open()
{
	GPIO_RESET(LED_RED);
	//GPIO_SET(LED_GREEN);
}

void LEN_RED_Close()
{
	GPIO_SET(LED_RED);
}

void LEN_GREEN_Open()
{
	GPIO_RESET(LED_GREEN);
	//GPIO_SET(LED_RED);
}

void LEN_GREEN_Close()
{
	GPIO_SET(LED_GREEN);
}

void LEN_REDBEEP_Open()
{
	LEN_RED_Open();
	BeepStart();
}

void LEN_REDBEEP_Close()
{
	LEN_RED_Close();
	 BeepStop();
}

void LEN_GREENBEEP_Open()
{
	LEN_GREEN_Open();
	BeepStart();
}

void LEN_GREENBEEP_Close()
{
	LEN_GREEN_Close();
	 BeepStop();
}

//中断中快闪
void LedSharpInIT(u8* time,bool success,u32 systime,u32 ontime,u32 offtime)
{   
   	static u32 sys = 0; 
	
	if(*time)
	{ 
	    if(success)
	    {
		if(systime > sys)
		{
		    if(*time & 0x01)
		    {
			//debug("LEN_GREEN_Close \r\n");
			LEN_GREEN_Close();
			sys = systime + offtime;
		    }
		    else 
		    {
			//debug("LEN_GREEN_Open \r\n");
			LEN_GREEN_Open();
			sys = systime + ontime;
		    }
		    (*time) --;
		}
	    		
	    }else
	    {
		if(systime > sys)
		{
		    if(*time & 0x01)
		    {
			LEN_RED_Close();
			sys = systime + offtime;
		    }
		    else 
		    {
			LEN_RED_Open();
			sys = systime + ontime;
		    }
		    (*time) --;
		}
	    }
	    
	    if(*time == 0) {LEN_GREEN_Close();LEN_RED_Close();sys = 0;}
	}
}

