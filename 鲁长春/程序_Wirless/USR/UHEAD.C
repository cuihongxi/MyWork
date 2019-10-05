#include "UHEAD.H"



void LSI_delayus(unsigned int  nCount)   //16M 晶振时  延时 1个微妙
{
    nCount*=3;
    while(--nCount);
}

void LSI_delayms(unsigned int  nCount)  //16M 晶振时  延时 1个毫秒
{
    while(nCount--)
    {
        LSI_delayus(1000);      
    }
}

