/**
 * ESP8266涓撶敤鐨勫欢鏃跺嚱鏁�
 * 2019骞�7鏈�13鏃�17:35:11
 */
#ifndef __delay_h
#define __delay_h

#include "osapi.h"


#define	delay_us(time)  os_delay_us(time)

void 	delay_ms(unsigned int time);

#endif
