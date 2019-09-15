#include "UHEAD.H"
#include "CUI_RTOS.H"



void LSI_delayus(unsigned int  nCount)   //16M ¾§ÕñÊ±  ÑÓÊ± 1¸öÎ¢Ãî
{
    nCount*=3;
    while(--nCount);
}

void LSI_delayms(unsigned int  nCount)  //16M ¾§ÕñÊ±  ÑÓÊ± 1¸öºÁÃë
{
    while(nCount--)
    {
        LSI_delayus(1000);      
    }
}

bool Juge_counter(JugeCStr* juge, u32 swdat)
{
	if(juge->start)
	{
		juge->counter += IRQ_PERIOD;
		if(juge->counter > swdat)
		{
			juge->counter = 0;
			juge->switchon = 1;
			juge->start = 0;
			return (bool)TRUE;
		}
	}
	return (bool)FALSE;
}