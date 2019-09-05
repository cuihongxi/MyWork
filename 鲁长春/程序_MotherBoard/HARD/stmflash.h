#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "UHEAD.h"


#define		FLASH_INIT_DATA				0x12			// ��ʼ��ֵ
#define		PASSWORD_SEC			60*60*24*30		// ����ʱ��,��λ��,=0����ʱ�䵽,==0xffffffff���ý���

#define		SUPER_PASSWORD		265500  			// ��������,��ʾ�����û�	


#define         ADDR_DM    			0x1000  // ���屣���ַ 0x1000��ʼ��dm_counter
#define         ADDR_AM_VAL  		0x1010  // AM״̬ key_AM.val
#define         ADDR_motorIO     	0x1020  // flag_motorIO
#define         ADDR_shut_time     	0x1030  // ����ش�ʱ��

#define         EEPROM_ADDRESS2     0x1040  // ����NRFͨѶ��ַ
#define         EEPROM_ADDRESS3     0x1050  // ����NRFͨѶ��ַ
#define         EEPROM_ADDRESS4     0x1060  // ����NRFͨѶ��ַ
#define         EEPROM_BK           0x1070  // ���������
#define			FLASH_INIT_SECTOR	0x1080	// ����FLASH��ʼ���ı�־ֵ


void FlashData_Init();

#endif
