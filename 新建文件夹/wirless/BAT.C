#include "BAT.H"
#include "ADC_CHECK.H"

uint16_t Get_BAT_ADC_Dat(hardChannel hard_channel)
{
	uint16_t dat = 0;
	GPIO_RESET(BatControl_GPIO);
	dat = Get_ADC_Dat( hard_channel);
	GPIO_SET(BatControl_GPIO);
	return dat;
}



