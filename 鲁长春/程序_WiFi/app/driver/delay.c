#include "delay.h"


void delay_ms(unsigned int time)
{
    for(;time>0;time --) delay_us(1000);
}
