#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "UHEAD.h"


#define		FLASH_INIT_DATA		0x13			// 初始化值


#define         ADDR_DM    		0x1000  // 定义保存地址 0x1000开始，dm_counter
#define         ADDR_AM_VAL  		0x1010  // AM状态 key_AM.val
#define         ADDR_motorIO     	0x1020  // flag_motorIO
#define         ADDR_flag_BH     	0x1030  // BH正方向

#define         EEPROM_ADDRESS2     	0x1040  // 保存NRF通讯地址
#define         EEPROM_ADDRESS3     	0x1050  // 保存NRF通讯地址
#define         EEPROM_ADDRESS4     	0x1060  // 保存NRF通讯地址
#define         EEPROM_BK           	0x1070  // 保存后背密码
#define		EEPROM_FLASH_INIT	0x1080	// 保存FLASH初始化的标志值


void FlashData_Init();

#endif
