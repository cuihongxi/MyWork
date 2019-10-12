/**
 * 2019年10月11日14:35:10
 * FLASH 函数
 * 注意：必须4字节为单位进行读写
 */

#ifndef __MYFLASH_H_
#define __MYFLASH_H_

#include "UHEAD.H"
#include "spi_flash.h"
#include "user_interface.h"


typedef unsigned char errorFlash;

#define		FLASH_MAX_SIZE			0X3FFFFF        // 字节，4M
#define     FLASH_SECTOR_SIZE   	0X1000          // 4096字节，一个扇区
#define     FLASH_ADDR_BEGIN    	0X70000         // 起始地址
//#define     FLASH_UNIT              4               // 4字节为单位读写

/*移植定义*/
#define	    FLASH_ERASE(a)  	                            spi_flash_erase_sector((uint16)(a/FLASH_SECTOR_SIZE))   // FLASH擦除扇区
#define     FLASH_WRITE_SECTORS(des_addr,src_addr,size)     spi_flash_write(des_addr,(uint32*)src_addr,size)        // FLASH写扇区
#define     FLASH_READ_SECTORS(des_addr,src_addr,size)      spi_flash_read(des_addr,(uint32*)src_addr,size)         // FLASH读扇区
#define     MemCopy                       					os_memcpy		// 内存拷贝


/*错误报警*/
#define     ERRO_FLASH_OK             0    // ok
#define     ERRO_FLASH_RESULT_ERR     1    // RESULT_ERR
#define     ERRO_FLASH_TIMEOUT        2    // TIMEOUT
#define     ERRO_FLASH_OUTRANGE       0x10 // 超出FLASH上限


/* 函数 */

errorFlash Flash_Read(uint32 des_addr , u8* src_addr,uint32 size);
errorFlash Flash_Write(uint32 des_addr , u8* src_addr,uint32 size);


#endif

