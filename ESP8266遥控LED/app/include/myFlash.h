/**
 * 2019��10��11��14:35:10
 * FLASH ����
 * ע�⣺����4�ֽ�Ϊ��λ���ж�д
 */

#ifndef __MYFLASH_H_
#define __MYFLASH_H_

#include "UHEAD.H"
#include "spi_flash.h"
#include "user_interface.h"


typedef unsigned char errorFlash;

#define		FLASH_MAX_SIZE			0X3FFFFF        // �ֽڣ�4M
#define     FLASH_SECTOR_SIZE   	0X1000          // 4096�ֽڣ�һ������
#define     FLASH_ADDR_BEGIN    	0X70000         // ��ʼ��ַ
//#define     FLASH_UNIT              4               // 4�ֽ�Ϊ��λ��д

/*��ֲ����*/
#define	    FLASH_ERASE(a)  	                            spi_flash_erase_sector((uint16)(a/FLASH_SECTOR_SIZE))   // FLASH��������
#define     FLASH_WRITE_SECTORS(des_addr,src_addr,size)     spi_flash_write(des_addr,(uint32*)src_addr,size)        // FLASHд����
#define     FLASH_READ_SECTORS(des_addr,src_addr,size)      spi_flash_read(des_addr,(uint32*)src_addr,size)         // FLASH������
#define     MemCopy                       					os_memcpy		// �ڴ濽��


/*���󱨾�*/
#define     ERRO_FLASH_OK             0    // ok
#define     ERRO_FLASH_RESULT_ERR     1    // RESULT_ERR
#define     ERRO_FLASH_TIMEOUT        2    // TIMEOUT
#define     ERRO_FLASH_OUTRANGE       0x10 // ����FLASH����


/* ���� */

errorFlash Flash_Read(uint32 des_addr , u8* src_addr,uint32 size);
errorFlash Flash_Write(uint32 des_addr , u8* src_addr,uint32 size);


#endif

