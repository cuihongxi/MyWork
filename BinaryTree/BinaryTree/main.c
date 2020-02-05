#include "hufman.h"
#include <stdio.h>

u32 array[256] = {0};

int main(void)
{
	powStr* ps;
	SortData(array, "111111222233344556",18);
	ps = BulidHufmanTree(array);
    printf("ps->power = %d\r\n",ps->power);
	ps = ps->bs->right;
	printf("ps->power = %d\r\n",ps->power);
	while(ps->bs->left)
	{
		ps = ps->bs->left;
		printf("ps->power = %d\r\n",ps->power);		
	}
	 printf("right end\r\n");
	return 0;

}
