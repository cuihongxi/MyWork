/******************************************************************
2019年9月29日07:05:45
内存分配方案1：
适用于固定大小内存分配
内存申请是1个单位，释放也是一个单位

*/


#ifndef     __CUI_MALLOC1_H
#define     __CUI_MALLOC1_H


#include "UHEAD.H"

#define     DEBUG_Malloc_LEVEL  0  // 调试开关

#define     Malloc_UNIT       15//(Malloc_MINI*Malloc_MINIINDEX+8)        // 大字节分配的最小单元
#define     Malloc_MAXSIZE    1200      // 分配最大内存空间，必须是Malloc_UNIT*8的整数倍,1056

#define		INDEX_NUM	 		10 //(Malloc_MAXSIZE/Malloc_UNIT)>>3)


#if   DEBUG_Malloc_LEVEL >0
#include<stdio.h>
#endif

#if (DEBUG_Malloc_LEVEL > 0)
#define  Malloc_Log(...)         printf(__VA_ARGS__);
#else
#define Malloc_Log(...)   
#endif                             


void* CUI_MALLOC1_OneUnit();                   		// 申请内存
Flag_Status  FreeCUI_MALLOC1(void* Malloc);        // 释放内存
// u32 GetNumMalloc(u8* Malloc);                   // 获取该malloc的数量
#endif
