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
	// printf(" map[1] =  %d\r\n",map.tab[49]);
	// printf(" map[2] =  %d\r\n",map.tab[50]);
	// printf(" map[3] =  %d\r\n",map.tab[51]);
	// printf(" map[4] =  %d\r\n",map.tab[52]);
	// printf(" map[7] =  %d\r\n",map.tab[59]);
	return 0;

}
