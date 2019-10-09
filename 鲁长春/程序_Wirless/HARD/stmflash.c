#include "stmflash.h"
#include "stm8l15x_flash.h"


extern	u8		is_DM;

//初始化表头的数据
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 					// 解锁EEPROM
	if(FLASH_ReadByte(EEPROM_FLASH_INIT) != FLASH_INIT_DATA)		//如果没有被初始化就初始化FLASH
	{
		FLASH_ProgramByte(EEPROM_FLASH_INIT,FLASH_INIT_DATA);	
		FLASH_ProgramByte(ADDR_DM,NO_DM);				//默认关闭，AM
	}

	is_DM = FLASH_ReadByte(ADDR_DM);
	debug("is_DM = %d\r\n",is_DM);
}



