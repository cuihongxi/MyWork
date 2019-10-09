#include "stmflash.h"
#include "stm8l15x_flash.h"


extern	u8		is_DM;

//��ʼ����ͷ������
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 					// ����EEPROM
	if(FLASH_ReadByte(EEPROM_FLASH_INIT) != FLASH_INIT_DATA)		//���û�б���ʼ���ͳ�ʼ��FLASH
	{
		FLASH_ProgramByte(EEPROM_FLASH_INIT,FLASH_INIT_DATA);	
		FLASH_ProgramByte(ADDR_DM,NO_DM);				//Ĭ�Ϲرգ�AM
	}

	is_DM = FLASH_ReadByte(ADDR_DM);
	debug("is_DM = %d\r\n",is_DM);
}



