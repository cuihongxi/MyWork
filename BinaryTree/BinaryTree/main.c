#include "hufman.h"
#include <stdio.h>


int main(void)
{
	mapTabStr* map;
	u8 hufmanfile[30] = {0};
	u8 encodefile[30] = {0};
	u8 i = 0;

	map = HufmanCompressFile("111111222233344556",StrGetLength("111111222233344556")+1,hufmanfile); 
	for(i = 0;i<30;i++)
	{
		printf("%#x	",hufmanfile[i]);
	}
	printf("\r\n");
	HufmanUncompressFile(hufmanfile,map,encodefile);
	printf("encodefile = %s\r\n",encodefile);
	printf("\r\n end !!!\r\n");
	return 0;

}
