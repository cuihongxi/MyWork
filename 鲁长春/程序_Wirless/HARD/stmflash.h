#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "UHEAD.h"


#define		FLASH_INIT_DATA		0x22			// 初始化值


#define		NO_DM			0	// 没有配对
#define		YES_DM			1	// 已配对
#define         ADDR_DM    		0x1000  // 保存是否配对信息

#define         EEPROM_ADDRESS0     	0x1020 
#define         EEPROM_ADDRESS1     	0x1030 
#define         EEPROM_ADDRESS2     	0x1040 
#define         EEPROM_ADDRESS3     	0x1050  
#define         EEPROM_ADDRESS4     	0x1060  
#define         EEPROM_BK           	0x1070 
#define		EEPROM_FLASH_INIT	0x1080	// 保存FLASH初始化的标志值


void FlashData_Init();

#endif
