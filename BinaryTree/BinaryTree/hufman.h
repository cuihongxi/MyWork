/**
 * 2020年2月4日16:33:43
 * 霍夫曼算法 
 * 步骤：
 * ①    创建一个全局变量数组 u32 array[256] = {0},该数组保存要压缩文件的权重。
 * ②    反复调用 SortData(u32* array, u8* file,u32 length) 将文件file读完。最终array保存了要压缩文件的各个字节权重。
 * ③    调用 powStr* BulidHufmanTree(u32* array) 创建一颗哈夫曼树，返回这棵树的权重节点指针
            
 */

#ifndef __HUFMAN_H
#define __HUFMAN_H

#include "uhead.h"
#include "BinaryTree.h"

// 权重节点
typedef struct{
    btreeStr*  bs;
	u8	power;           // 权重， 如果以字节为单位对数据进行压缩，每个叶子节点的数据权重为unsigned char类型
}powStr;

//叶子节点
typedef struct{
    powStr pow;        
    u8  value;          // 节点的值
    u8* code;           // 编码字符串
}leafStr;

typedef struct 
{
    u8 power;
    u8 value;
}valStr;





void SortData(u32* array, u8* file,u32 length); // 读取文件file,u32 array[256] 数组值记录权重power，下标记录value
powStr* BulidHufmanTree(u32* array);            // 创建树    

#endif // !__HUFMAN_H