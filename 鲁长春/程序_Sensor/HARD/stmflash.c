#include "stmflash.h"
#include "stm8l15x_flash.h"
#include "24l01.h"
/*
	stm8l051从0x1000开始，最多256字节

*/
extern	u8		is_DM;

//初始化表头的数据
void FlashData_Init(addrNRFStr* buf)
{
	FLASH_Unlock(FLASH_MemType_Data); 						// 解锁EEPROM

	if(FLASH_ReadByte(ADDR_FLASH_INIT) != FLASH_INIT_DATA)	//如果没有被初始化就初始化FLASH
	{
		FLASH_ProgramByte(ADDR_FLASH_INIT,FLASH_INIT_DATA);	
		FLASH_ProgramByte(ADDR_INDEX,0);					//索引归零       
	}
	
	buf->index = FLASH_ReadByte(ADDR_INDEX);
	u8 i = 0;
	for(i=0;i<8;i++)
	{
		if(buf->index&(1<<i))
		{	  	
			buf->addr[i][0] = FLASH_ReadByte(ADDR_BASE + i*5);
			buf->addr[i][2] = FLASH_ReadByte(ADDR_BASE + i*5+1);
			buf->addr[i][3] = FLASH_ReadByte(ADDR_BASE + i*5+2);
			buf->addr[i][4] = FLASH_ReadByte(ADDR_BASE + i*5+3);
			buf->addr[i][5] = FLASH_ReadByte(ADDR_BASE + i*5+4);			
		}
		else break;
	}
}

// 遍历buf中有没有相同的地址，如果有返回0
u8 TraverseBuf(addrNRFStr* buf,u8* addr)
{
	u8 i = 0;
	for(i=0;i<8;i++)
	{
		if(buf->index&(1<<i))
		{	  	
			if(((*(u32*)&(buf->addr[i][0]))^(*(u32*)addr) | (buf->addr[i][5]^addr[5])) == 0) return 0;		
		}
		else return 1;
	}
	return 1;
}
// 保存新的配对信息
void FlashSaveNrfAddr(addrNRFStr* buf,u8* addr)
{
	if(buf->index < 0xff && TraverseBuf(buf,addr))
	{
		
		u8 i = 0;
		u8 j = 0;
		for(i=0;i<8;i++)
		{
			if(buf->index&(1<<i))j ++;
			else break;
		}		
		for(i=0;i<5;i++)
		{
			buf->addr[j][i] = addr[i];
	  		FLASH_ProgramByte(ADDR_BASE + j*5 + i,addr[i]);
		}
		
		buf->index = (buf->index >> 1) + 1;
		FLASH_ProgramByte(ADDR_INDEX,buf->index);
	}
}

// 清除配对信息
void FlashClearDM(addrNRFStr* buf)
{
	buf->index = 0;
	FLASH_ProgramByte(ADDR_INDEX,buf->index);
}

