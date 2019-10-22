#include "myFlash.h"

/**
*   ��ĳ����ַ�������ݣ������ֵ�src_addrΪ�׵ĵ�ַ��
*   des_addrΪ��Ե�ַ����0~FLASH_MAX_SIZE��
*/ 
errorFlash  Flash_Read(uint32 des_addr , u8* src_addr,uint32 size)
{
    u8 erro = ERRO_FLASH_OK;
    if((des_addr + size) > FLASH_MAX_SIZE) return ERRO_FLASH_OUTRANGE;
    uint32 startSector = FLASH_ADDR_BEGIN + (des_addr/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE;    // ������ʼ������ַ
    uint32 startoffset = des_addr&(FLASH_SECTOR_SIZE - 1);                                     // ���������е�ƫ����
    uint32 globelBody = 0;
    if((size + startoffset) > FLASH_SECTOR_SIZE)
    	globelBody = (size + startoffset - FLASH_SECTOR_SIZE)/FLASH_SECTOR_SIZE;             // ������������������
    uint32 endOffset = (size + startoffset)&(FLASH_SECTOR_SIZE - 1);                           // ����ĩβ�ֽ�ƫ��

    /* ������ */
    u8 datbuf[FLASH_SECTOR_SIZE] = {0};
    erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);    // ������
    if((startoffset + size) > FLASH_SECTOR_SIZE)                // ����һ������
    {
        MemCopy(src_addr,datbuf + startoffset,FLASH_SECTOR_SIZE - startoffset);
        src_addr += FLASH_SECTOR_SIZE - startoffset;
        debug("write : globelBody = %d\n",globelBody);
        while(globelBody --)
        {
            startSector += FLASH_SECTOR_SIZE;
            erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);    // ������
            MemCopy((u8*)src_addr,(u8*)datbuf,FLASH_SECTOR_SIZE); 
            src_addr += FLASH_SECTOR_SIZE;
        }
        startSector += FLASH_SECTOR_SIZE;
        erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);    // ������
        MemCopy((u8*)src_addr,(u8*)datbuf,endOffset); 
    }else
    {
        MemCopy(src_addr,datbuf + startoffset,size);
    }
    
    return erro;
}

/**
 *  ��src_addr��ַ�����ݣ�д��des_addr
*   des_addrΪFLASH��Ե�ַ����0~FLASH_MAX_SIZE��
*/ 

errorFlash  Flash_Write(uint32 des_addr , u8* src_addr,uint32 size)
{
    u8 erro = ERRO_FLASH_OK;
    if((des_addr + size) > FLASH_MAX_SIZE) return ERRO_FLASH_OUTRANGE;
    uint32 startSector = FLASH_ADDR_BEGIN + (des_addr/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE;    	// ������ʼ������ַ
    uint32 startoffset = des_addr&(FLASH_SECTOR_SIZE - 1);                                     	// ���������е�ƫ����
    uint32 globelBody = 0;
    if((size + startoffset) > FLASH_SECTOR_SIZE)
    	globelBody = (size + startoffset - FLASH_SECTOR_SIZE)/FLASH_SECTOR_SIZE;             // ������������������
    uint32 endOffset = (size + startoffset)&(FLASH_SECTOR_SIZE - 1);                           	// ����ĩβ�ֽ�ƫ��

    /* д���� */
    u8 datbuf[FLASH_SECTOR_SIZE] = {0};
    erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);                // ������
    FLASH_ERASE(startSector);                                                                   // ��������

    if((startoffset + size) > FLASH_SECTOR_SIZE)                                                // ����һ������
    {
        MemCopy((u8*)datbuf + startoffset,(u8*)src_addr ,FLASH_SECTOR_SIZE - startoffset);      // װ������ 
        erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);           // д������
        src_addr += FLASH_SECTOR_SIZE - startoffset;
        debug("write : globelBody = %d\n",globelBody);
        while(globelBody --)
        {
            startSector += FLASH_SECTOR_SIZE;
            erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);          // ������
            FLASH_ERASE(startSector);                                                            // ��������
            MemCopy((u8*)datbuf,(u8*)src_addr,FLASH_SECTOR_SIZE);                                // װ������  
            erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);        // д������
            src_addr += FLASH_SECTOR_SIZE;
        }
        startSector += FLASH_SECTOR_SIZE;
        erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);             // ������
        FLASH_ERASE(startSector);                                                               // ��������
        MemCopy((u8*)datbuf,(u8*)src_addr,endOffset);                                           // װ������  
        erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);           // д������

    }
    else
    {
        MemCopy((u8*)datbuf + startoffset,(u8*)src_addr ,size);                                  // װ������
        erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);            // д������
    }
    
    return erro;

}
