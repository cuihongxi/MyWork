#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "UHEAD.h"


#define		FLASH_INIT_DATA		0x24			// 初始化值

#define			ADDR_FLASH_INIT		0x1000		// 保存FLASH初始化的标志值
#define			ADDR_INDEX			0x1001		// 保存索引的地址
#define			ADDR_BASE			0x1002		// 配对信息的基地址

typedef struct{
  	u8 addr[8][5];
	u8 index;
}addrNRFStr;

void FlashData_Init(addrNRFStr* buf);

void FlashClearDM(addrNRFStr* buf);// 清除配对信息
void FlashSaveNrfAddr(addrNRFStr* buf,u8* addr);// 保存新的配对信息

#endif
