#include "CUI_MALLOC0.H"
u8 CUI_Malloc0_Array[Malloc0_MAXSIZE] = {0};

CUI_Malloc0MesgStr malloc0Mesg = {CUI_Malloc0_Array,Malloc0_MAXSIZE,CUI_Malloc0_Array};

 void* CUI_MALLOC0(CUI_Malloc0MesgStr* mallocMesg , u32  leng_th)
 {
	 
 	if((mallocMesg->addr + leng_th) <= (mallocMesg->array + mallocMesg->size))
	{
		u8* addr = mallocMesg->addr;
		mallocMesg->addr += leng_th;
		Malloc0_Log("已经申请Malloc0内存,地址：0x%x,(mallocMesg->array + mallocMesg->size) = 0x%x\r\n",addr,(mallocMesg->array + mallocMesg->size));
		return (void*)addr;
	}else return 0;
 }