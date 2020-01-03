#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "UHEAD.h"


#define		FLASH_INIT_DATA		0x24			// ��ʼ��ֵ

#define			ADDR_FLASH_INIT		0x1000		// ����FLASH��ʼ���ı�־ֵ
#define			ADDR_INDEX			0x1001		// ���������ĵ�ַ
#define			ADDR_BASE			0x1002		// �����Ϣ�Ļ���ַ

typedef struct{
  	u8 addr[8][5];
	u8 index;
}addrNRFStr;

void FlashData_Init(addrNRFStr* buf);

void FlashClearDM(addrNRFStr* buf);// ��������Ϣ
void FlashSaveNrfAddr(addrNRFStr* buf,u8* addr);// �����µ������Ϣ

#endif
