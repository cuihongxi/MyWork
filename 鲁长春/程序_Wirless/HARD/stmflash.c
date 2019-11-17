#include "stmflash.h"
#include "stm8l15x_flash.h"
#include "24l01.h"

extern	u8		is_DM;

//初始化表头的数据
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 					// 解锁EEPROM
//	if(FLASH_ReadByte(EEPROM_FLASH_INIT) != FLASH_INIT_DATA)		//如果没有被初始化就初始化FLASH
//	{
//		FLASH_ProgramByte(EEPROM_FLASH_INIT,FLASH_INIT_DATA);	
//		FLASH_ProgramByte(ADDR_DM,NO_DM);				//默认关闭，AM
//                FLASH_ProgramByte(EEPROM_ADDRESS0,ADDRESS1[0]);
//                FLASH_ProgramByte(EEPROM_ADDRESS1,ADDRESS1[1]);
//                FLASH_ProgramByte(EEPROM_ADDRESS2,ADDRESS1[2]);
//                FLASH_ProgramByte(EEPROM_ADDRESS3,ADDRESS1[3]);
//                FLASH_ProgramByte(EEPROM_ADDRESS4,ADDRESS1[4]);
//                
//	}
        ADDRESS2[0] = FLASH_ReadByte(EEPROM_ADDRESS0);
        ADDRESS2[1] = FLASH_ReadByte(EEPROM_ADDRESS1);
        ADDRESS2[2] = FLASH_ReadByte(EEPROM_ADDRESS2);
        ADDRESS2[3] = FLASH_ReadByte(EEPROM_ADDRESS3);
        ADDRESS2[4] = FLASH_ReadByte(EEPROM_ADDRESS4);

        
}



