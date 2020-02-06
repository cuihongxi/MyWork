#include "hufman.h"
#include <stdio.h>

u32 array[256] = {0};
mapTabStr map = {0};
u8 str[40] = {0};
int main(void)
{
	powStr* ps;
	SortData(array, "111111222233344556",18);
	ps = BulidHufmanTree(array);
    // printf("ps->power = %d\r\n",ps->power);
	// ps = ps->bs->right;
	// printf("ps->power = %d\r\n",ps->power);
	// while(ps->bs->left)
	// {
	// 	ps = ps->bs->left;
	// 	printf("ps->power = %d\r\n",ps->power);		
	// }
	//  printf("right end\r\n");
	TabHufmanCreat(ps,&map,str); // ½âÎöÊ÷£¬±£´æÓ³Éä±í
	 printf(" map[1] =  %s\r\n",map.tab[54]);
	 printf(" map[2] =  %s\r\n",map.tab[52]);
	 printf(" map[3] =  %s\r\n",map.tab[53]);
	 printf(" map[4] =  %s\r\n",map.tab[51]);
	 printf(" map[7] =  %s\r\n",map.tab[50]);
	return 0;

}
