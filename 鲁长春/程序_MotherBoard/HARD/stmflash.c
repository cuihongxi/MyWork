#include "stmflash.h"
#include "stm8l15x_flash.h"
#include "keyboard.h"

extern u8	flag_motorIO;
extern u32	dm_counter;
//初始化表头的数据
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 							// 解锁EEPROM
	if(FLASH_ReadByte(FLASH_INIT_SECTOR) != FLASH_INIT_DATA)	//如果没有被初始化就初始化FLASH
	{
		FLASH_ProgramByte(FLASH_INIT_SECTOR,FLASH_INIT_DATA);	

		FLASH_ProgramWord(ADDR_DM,5000);				//默认5s
//		FLASH_ProgramByte(ADDR_AM_VAL,0);				//默认关闭，AM
		FLASH_ProgramByte(ADDR_motorIO,0);				//马达引脚调换
	}
	dm_counter = FLASH_ReadWord(ADDR_DM);	
//	key_AM.val = (keyEnum)FLASH_ReadByte(ADDR_AM_VAL);
	flag_motorIO = FLASH_ReadByte(ADDR_motorIO);

}



