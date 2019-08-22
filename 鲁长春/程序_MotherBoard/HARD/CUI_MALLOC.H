/******************************************************************
*文件名:公共头文件
*创建者:探长科技，www.explorersoftware.taobao.com
*创建时间:2017年8月9日14:07:03
*文件说明:动态分配内存
*2019年5月7日17:16:22
*	增加小字节的动态内存分配
分配方法是：分配一个大小为最小大字节单元作为小字节池，
						最后的8个字节作为特殊功能码，前四字节记录下一个小字节分配池的指针，没有则为0x00000000
						后四个字节作为索引表，按照之前的索引规则进行分配。
*/


#ifndef     __CUI_MALLOC_H
#define     __CUI_MALLOC_H


#include "UHEAD.H"

#define     DEBUG_Malloc_LEVEL  0  // 调试开关

#define		Malloc_MINI				4								// 小字节分配的最小单元
#define		Malloc_MINIINDEX		31								// 索引数，以字节位数减一，7,15,32
#define     Malloc_UNIT       (Malloc_MINI*Malloc_MINIINDEX+8)        // 大字节分配的最小单元
#define     Malloc_MAXSIZE    8*Malloc_UNIT       // 分配最大内存空间，必须是Malloc_UNIT*8的整数倍,1056


#if   DEBUG_Malloc_LEVEL >0
#include<stdio.h>
#endif

#if (DEBUG_Malloc_LEVEL > 0)
#define  Malloc_Log(...)         printf(__VA_ARGS__);
#else
#define Malloc_Log(...)   
#endif                             

typedef enum {ISOK = 0, ISERROR = 1} Flag_Status;


// 小字节内存池数据结构
typedef struct{
u8 dat[Malloc_MINI*Malloc_MINIINDEX];		// 数据区
u8* pNext;									// 指向下一个小字节池地址
u32 index;									// 索引表						
}MiniMallocStr;

void* CUI_MALLOC(u32  leng_th);                   // 申请内存
Flag_Status  FreeCUI_MALLOC(void* Malloc);        // 释放内存
// u32 GetNumMalloc(u8* Malloc);                   // 获取该malloc的数量
#endif
