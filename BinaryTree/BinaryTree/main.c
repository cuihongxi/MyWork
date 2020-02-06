#include "hufman.h"
#include <stdio.h>

// mapTabStr* map = 0;

int main(void)
{
	mapTabStr* map;
	u8 datbuf[30] = {0};
	u8 i = 0;
	u8 dat = 0;
	u32 datbuf_byte = 0;
	u8 datbuf_bit = 0;
	//mapTabStr map = {0};
//	powStr* ps;
//	SortData(array, "111111222233344556",18);
//	ps = BulidHufmanTree(array);

//	ps = BulidHufmanTreeOneTime("111111222233344556",StrGetLength("111111222233344556"));
//	_TabHufmanCreat(ps,&map); // ½âÎöÊ÷£¬±£´æÓ³Éä±í

	// map = BulidHufmanTabForStr("111111222233344556",StrGetLength("111111222233344556")+1);
	// printf(" map[1] =  %s\r\n",map->tab[54]);
	// printf(" map[2] =  %s\r\n",map->tab[52]);
	// printf(" map[3] =  %s\r\n",map->tab[53]);
	// printf(" map[4] =  %s\r\n",map->tab[51]);
	// printf(" map[7] =  %s\r\n",map->tab[50]);

	// Free_HufmanTree(map->hafmanTree);
	
	map = HufmanCompressFile("111111222233344556",StrGetLength("111111222233344556")+1,datbuf); 
	for(i = 0;i<30;i++)
	{
		printf("%#x	",datbuf[i]);
	}
	i = UncopressByte(map,&dat,&datbuf_byte,&datbuf_bit);
	printf("i = %d\r\n",i);
	printf("\r\n end !!!\r\n");
return 0;

}
