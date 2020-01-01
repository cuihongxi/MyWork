/******************************************************************
2019年9月29日07:01:42
内存分配方案0：
适用于不释放或者整体释放的场合

*/


#ifndef     __CUI_MALLOC0_H
#define     __CUI_MALLOC0_H


#include "UHEAD.H"

#define     DEBUG_Malloc0_LEVEL  0  // 调试开关

#if   DEBUG_Malloc0_LEVEL >0
#include<stdio.h>
#endif

#if (DEBUG_Malloc0_LEVEL > 0)
#define  Malloc0_Log(...)         printf(__VA_ARGS__);
#else
#define Malloc0_Log(...)   
#endif                             


#define     Malloc0_MAXSIZE    140      // 分配最大内存空间

//该模式的内存状态
typedef struct{
	u8* array;		// 数组地址
	u32 size;		// 数组大小
	u8* addr;		// 未分配的起始地址
}CUI_Malloc0MesgStr;

extern CUI_Malloc0MesgStr malloc0Mesg;
void* CUI_MALLOC0(CUI_Malloc0MesgStr* mallocMesg , u32  leng_th);        	// 申请内存

#endif
