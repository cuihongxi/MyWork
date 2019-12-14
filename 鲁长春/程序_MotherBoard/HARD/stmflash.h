#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "UHEAD.h"


#define		FLASH_INIT_DATA		0x93			// ��ʼ��ֵ


#define         ADDR_DM    			0x1000  // ���屣���ַ 0x1000��ʼ��flag_DM
#define         ADDR_AM_VAL  		0x1010  // AM״̬ key_AM.val
#define         ADDR_motorIO     	0x1020  // flag_motorIO
#define         ADDR_flag_BH     	0x1030  // BH������
#define         EEPROM_BK           0x1070  // ���������
#define			EEPROM_FLASH_INIT	0x1080	// ����FLASH��ʼ���ı�־ֵ

#define         EEPROM_ADDRESS2     	0x1040 
#define         EEPROM_ADDRESS3     	0x1050  
#define         EEPROM_ADDRESS4     	0x1060 
#define         EEPROM_ADDRESS0     	0x1090 
#define         EEPROM_ADDRESS1     	0x10A0 

#define         EEPROM_CGADDRESS2     	0x10B0 
#define         EEPROM_CGADDRESS3     	0x10C0  
#define         EEPROM_CGADDRESS4     	0x10D0 
#define         EEPROM_CGADDRESS0     	0x10E0 
#define         EEPROM_CGADDRESS1     	0x10F0 

void FlashData_Init();

#endif
