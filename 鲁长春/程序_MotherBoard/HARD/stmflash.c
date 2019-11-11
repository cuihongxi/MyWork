#include "stmflash.h"
#include "stm8l15x_flash.h"
#include "keyboard.h"

extern 	u8		flag_DM;

//��ʼ����ͷ������
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 							// ����EEPROM
	if(FLASH_ReadByte(EEPROM_FLASH_INIT) != FLASH_INIT_DATA)	//���û�б���ʼ���ͳ�ʼ��FLASH
	{
		FLASH_ProgramByte(EEPROM_FLASH_INIT,FLASH_INIT_DATA);	
		FLASH_ProgramByte(ADDR_DM,0);				//������ŵ���

	}

	flag_DM = FLASH_ReadByte(ADDR_DM);

	
}



