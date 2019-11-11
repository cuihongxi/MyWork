#include "stmflash.h"
#include "stm8l15x_flash.h"
#include "keyboard.h"

extern 	u8		flag_DM;

//初始化表头的数据
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 							// 解锁EEPROM
	if(FLASH_ReadByte(EEPROM_FLASH_INIT) != FLASH_INIT_DATA)	//如果没有被初始化就初始化FLASH
	{
		FLASH_ProgramByte(EEPROM_FLASH_INIT,FLASH_INIT_DATA);	
		FLASH_ProgramByte(ADDR_DM,0);				//马达引脚调换

	}

	flag_DM = FLASH_ReadByte(ADDR_DM);

	
}



