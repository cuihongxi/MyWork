#include "sort.h"

// 冒泡排序,从大到小顺序
void BubblSortBig2Small(u8* array,u32 len)
{
    u32 i = 0;
    u32 j = len -1;
    while(j != 0)
    {
        for(i=0;i<j;i++)
        {
            if(array[i]<array[i+1])
            {
                array[i] ^= array[i+1];
                array[i+1] ^= array[i];
                array[i] ^= array[i+1];
            }
        }
        j --;
    }
}


