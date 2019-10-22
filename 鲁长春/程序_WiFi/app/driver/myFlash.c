#include "myFlash.h"

/**
*   从某个地址读出数据，并保持到src_addr为首的地址中
*   des_addr为相对地址：（0~FLASH_MAX_SIZE）
*/ 
errorFlash  Flash_Read(uint32 des_addr , u8* src_addr,uint32 size)
{
    u8 erro = ERRO_FLASH_OK;
    if((des_addr + size) > FLASH_MAX_SIZE) return ERRO_FLASH_OUTRANGE;
    uint32 startSector = FLASH_ADDR_BEGIN + (des_addr/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE;    // 计算起始扇区地址
    uint32 startoffset = des_addr&(FLASH_SECTOR_SIZE - 1);                                     // 计算扇区中的偏移量
    uint32 globelBody = 0;
    if((size + startoffset) > FLASH_SECTOR_SIZE)
    	globelBody = (size + startoffset - FLASH_SECTOR_SIZE)/FLASH_SECTOR_SIZE;             // 计算完整的扇区个数
    uint32 endOffset = (size + startoffset)&(FLASH_SECTOR_SIZE - 1);                           // 计算末尾字节偏移

    /* 读数据 */
    u8 datbuf[FLASH_SECTOR_SIZE] = {0};
    erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);    // 读扇区
    if((startoffset + size) > FLASH_SECTOR_SIZE)                // 超出一个扇区
    {
        MemCopy(src_addr,datbuf + startoffset,FLASH_SECTOR_SIZE - startoffset);
        src_addr += FLASH_SECTOR_SIZE - startoffset;
        debug("write : globelBody = %d\n",globelBody);
        while(globelBody --)
        {
            startSector += FLASH_SECTOR_SIZE;
            erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);    // 读扇区
            MemCopy((u8*)src_addr,(u8*)datbuf,FLASH_SECTOR_SIZE); 
            src_addr += FLASH_SECTOR_SIZE;
        }
        startSector += FLASH_SECTOR_SIZE;
        erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);    // 读扇区
        MemCopy((u8*)src_addr,(u8*)datbuf,endOffset); 
    }else
    {
        MemCopy(src_addr,datbuf + startoffset,size);
    }
    
    return erro;
}

/**
 *  从src_addr地址读数据，写入des_addr
*   des_addr为FLASH相对地址：（0~FLASH_MAX_SIZE）
*/ 

errorFlash  Flash_Write(uint32 des_addr , u8* src_addr,uint32 size)
{
    u8 erro = ERRO_FLASH_OK;
    if((des_addr + size) > FLASH_MAX_SIZE) return ERRO_FLASH_OUTRANGE;
    uint32 startSector = FLASH_ADDR_BEGIN + (des_addr/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE;    	// 计算起始扇区地址
    uint32 startoffset = des_addr&(FLASH_SECTOR_SIZE - 1);                                     	// 计算扇区中的偏移量
    uint32 globelBody = 0;
    if((size + startoffset) > FLASH_SECTOR_SIZE)
    	globelBody = (size + startoffset - FLASH_SECTOR_SIZE)/FLASH_SECTOR_SIZE;             // 计算完整的扇区个数
    uint32 endOffset = (size + startoffset)&(FLASH_SECTOR_SIZE - 1);                           	// 计算末尾字节偏移

    /* 写数据 */
    u8 datbuf[FLASH_SECTOR_SIZE] = {0};
    erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);                // 读扇区
    FLASH_ERASE(startSector);                                                                   // 擦除扇区

    if((startoffset + size) > FLASH_SECTOR_SIZE)                                                // 超出一个扇区
    {
        MemCopy((u8*)datbuf + startoffset,(u8*)src_addr ,FLASH_SECTOR_SIZE - startoffset);      // 装载数据 
        erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);           // 写入数据
        src_addr += FLASH_SECTOR_SIZE - startoffset;
        debug("write : globelBody = %d\n",globelBody);
        while(globelBody --)
        {
            startSector += FLASH_SECTOR_SIZE;
            erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);          // 读扇区
            FLASH_ERASE(startSector);                                                            // 擦除扇区
            MemCopy((u8*)datbuf,(u8*)src_addr,FLASH_SECTOR_SIZE);                                // 装载数据  
            erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);        // 写入数据
            src_addr += FLASH_SECTOR_SIZE;
        }
        startSector += FLASH_SECTOR_SIZE;
        erro |= FLASH_READ_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);             // 读扇区
        FLASH_ERASE(startSector);                                                               // 擦除扇区
        MemCopy((u8*)datbuf,(u8*)src_addr,endOffset);                                           // 装载数据  
        erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);           // 写入数据

    }
    else
    {
        MemCopy((u8*)datbuf + startoffset,(u8*)src_addr ,size);                                  // 装载数据
        erro |= FLASH_WRITE_SECTORS(startSector,datbuf,FLASH_SECTOR_SIZE);            // 写入数据
    }
    
    return erro;

}
