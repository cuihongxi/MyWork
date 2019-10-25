#include "stmflash.h"
#include "stm8l15x_flash.h"
#include "keyboard.h"

extern 	u8		flag_motorIO;
extern 	u32		dm_counter;
extern	u8		flag_BH;

//��ʼ����ͷ������
void FlashData_Init()
{
	FLASH_Unlock(FLASH_MemType_Data); 							// ����EEPROM
	if(FLASH_ReadByte(EEPROM_FLASH_INIT) != FLASH_INIT_DATA)	//���û�б���ʼ���ͳ�ʼ��FLASH
	{
		FLASH_ProgramByte(EEPROM_FLASH_INIT,FLASH_INIT_DATA);	
		FLASH_ProgramWord(ADDR_DM,5000);				//Ĭ��5s
//		FLASH_ProgramByte(ADDR_AM_VAL,0);				//Ĭ�Ϲرգ�AM
		FLASH_ProgramByte(ADDR_motorIO,1);				//������ŵ���
		FLASH_ProgramByte(ADDR_flag_BH,0);				// BH������
	}
	dm_counter = FLASH_ReadWord(ADDR_DM);	
//	key_AM.val = (keyEnum)FLASH_ReadByte(ADDR_AM_VAL);
	flag_motorIO = FLASH_ReadByte(ADDR_motorIO);
	flag_BH = FLASH_ReadByte(ADDR_flag_BH);
	
	debug("FLASH dm_counter = %lu\r\n",dm_counter);
}



