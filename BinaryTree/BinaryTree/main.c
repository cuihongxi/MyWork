#include "hufman.h"
#include <stdio.h>


int main(void)
{
	mapTabStr* map;
	// u8 hufmanfile[30] = {0};
	// u8 encodefile[30] = {0};
	u8 i = 0;

	// map = HufmanCompressFile("111111222233344556",StrGetLength("111111222233344556")+1,hufmanfile); 
	// for(i = 0;i<30;i++)
	// {
	// 	printf("%#x	",hufmanfile[i]);
	// }
	// printf("\r\n");
	// HufmanUncompressFile(hufmanfile,map,encodefile);
	// printf("encodefile = %s\r\n",encodefile);
//`map = HufmanCompress_CFile("C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\�����6.cui","C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\�����7.cui");
	
//	map = HufmanCompress_CFile("C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\��12�� STM32F1 GPIO����ԭ��.rmvb",\
//		"C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\��12�� STM32F1 GPIO����ԭ��.cui");

//		map = HufmanCompress_CFile("C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\��12�� STM32F1 GPIO����ԭ��.cui",\
//		"C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\2.cui");
	
	map = HufmanCompress_CFile("C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\LOGO.bmp","C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\LOGO.cui");
	HufmanUnompress_CFile(map,"C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\LOGO.cui","C:\\Users\\cuihongxi\\Pictures\\Feedback\\pic\\LOGO1.bmp");      // ��ѹ��
	printf("\r\n end !!!\r\n");
	return 0;

}
