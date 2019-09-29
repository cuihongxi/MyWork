#include "CUI_MALLOC1.H"

 u8  CUI_MALLOCArray[Malloc_MAXSIZE] = {0};            		// 申请数组
 u8  indexMalloc[INDEX_NUM] = {0};          				// 索引表


/************************************************************************************
*-函数名称	：动态申请一个单位内存
*-参数			：mallocArray 分配内存池的首地址；malloc_unit 最小分配单元
							malloc_maxsize 最大分配长度，本内存池大小 ;indexMalloc 索引表地址
*返回值			：		成功返回所申请的内存地址，不成功返回0
*-函数功能	：Malloc，动态申请内存 ,最后一位为0，做每次申请的结束符，清空时识别
*-创建者	：蓬莱市博硕电子科技
*/
u8* Malloc_OneUnit(u8* mallocArray,u32 malloc_unit,u32 malloc_maxsize,u8* indexMalloc)
{
	u8 array[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	//索引表的每个BIT对应一个单位内存，从头开始寻找没有被分配的位置
	u32 i =0;
	for(i =0;i<INDEX_NUM;i++)			
	{
		if(indexMalloc[i] != 0xff) break;
	}
	if(i >= INDEX_NUM) return 0;
	u32 j = 0;
	for(j = 0;j<8;j++)
	{
		if((indexMalloc[i] & array[j]) == 0)
		{
			indexMalloc[i] |= array[j];
			break;
		}
	}
	return &(mallocArray[(i*8+j)*malloc_unit]);
}

//u8* mymemcpy(u8* pd ,u8* ps ,u32 len)
//{
//	u32 i = 0;
//	u32 temp = len/sizeof(u32);
//	for(i=0;i<temp;i++) ((u32*)pd)[i] = ((u32*)ps)[i];
//	i *= sizeof(u32);
//	for(;i<len;i++) pd[i] = ps[i];
//	return &pd[len];
//}

//内存清零
void Clear_MM(u8* p,u32 leng_th)
{
	//for(u32 i=0;i<leng_th;i++)p[i] = 0;	
	u32 i = 0;
	u32 temp = leng_th/sizeof(u32);
	for(i=0;i<temp;i++) ((u32*)p)[i] = 0;
	i *= sizeof(u32);
	for(;i<leng_th;i++) p[i] = 0;
}
/************************************************************************************
*-函数名称	：动态申请内存
*-函数功能	：Malloc，动态申请内存一个单位的内存
*-创建者		：蓬莱市博硕电子科技
*/
void* CUI_MALLOC1_OneUnit(void)
{
	u8* p;
	p = Malloc_OneUnit(CUI_MALLOCArray,Malloc_UNIT,Malloc_MAXSIZE,indexMalloc);
	Malloc_Log("已申请 MALLOC1 地址：%#x \r\n",p);
	Clear_MM(p,Malloc_UNIT);
	return (void*)p;
}

/************************************************************************************
*-函数名称	：释放内存,通用函数
*-创建者	：蓬莱市博硕电子科技
*/
Flag_Status Free_MallocGE(u8* Malloc,u8* mallocArray,u32 malloc_unit,u32 malloc_maxsize,u8* indexMalloc)
{
	  u8 array[8] = {0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
	  u32 z = (Malloc - mallocArray);
      indexMalloc[z/120]  &= array[((u8)(z/malloc_unit))&7];//(~(0x80>>j));120 替换 (z/((u32)malloc_unit<<3)

//      if(i < (malloc_maxsize/malloc_unit+7)/8) 
	  		return ISOK;     
//      else
//      return ISERROR;
}

/************************************************************************************
*-函数名称	：释放内存
*-参数	：
*返回值	：成功返回OK，不成功返回0
*-函数功能	：释放动态生成的内存
*-创建者	：蓬莱市博硕电子科技
*/

Flag_Status    FreeCUI_MALLOC1(void* Malloc)
{
	Malloc_Log("释放 MALLOC1 地址: 0x%x\r\n",Malloc);
	return Free_MallocGE((u8*)Malloc,(u8*)&CUI_MALLOCArray,Malloc_UNIT,Malloc_MAXSIZE,indexMalloc);

}

