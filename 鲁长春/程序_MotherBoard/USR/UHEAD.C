#include "UHEAD.H"
#include "CUI_RTOS.H"



void LSI_delayus(unsigned int  nCount)   //16M ����ʱ  ��ʱ 1��΢��
{
    nCount*=3;
    while(--nCount);
}

void LSI_delayms(unsigned int  nCount)  //16M ����ʱ  ��ʱ 1������
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