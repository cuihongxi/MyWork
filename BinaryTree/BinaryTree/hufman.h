/**
 * 2020年2月4日16:33:43
 * 霍夫曼算法压缩及解压缩程序
 * 压缩步骤：
 * ①    创建一个全局变量数组 u32 array[256] = {0},该数组保存要压缩文件的权重。
 * ②    反复调用 SortData(u32* array, u8* file,u32 length) 将文件file读完。最终array保存了要压缩文件的各个字节权重。
 * ③    调用 powStr* BulidHufmanTree(u32* array) 创建一颗哈夫曼树，返回这棵树的权重节点指针
 * ④    解析树，创建映射表 ：
 *          建数组  mapTabStr map; 调用_TabHufmanCreat(hufmanTree,map)宏进行解析。该宏隐匿了一个全局变量tabhufmanstaticbuff
 *          
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
}leafStr;

//哈夫曼映射表
typedef struct 
{
    u8* tab[256];       // 保存字符串地址 
    powStr* hafmanTree; // 哈夫曼树
    u32 length;         // 所压缩文件的长度，byte为单位
}mapTabStr;

void TabHufmanCreat(powStr* hufmanTree,mapTabStr* map,u8* str); // 解析树，保存映射表
extern u8 tabhufmanstaticbuff ;
/* 用户函数 */
void SortData(u32* array, u8* file,u32 length);         // 读取文件file,u32 array[256] 数组值记录权重power，下标记录value
powStr* BulidHufmanTree(u32* array);                    // 创建树                
void Free_HufmanTree(powStr* hufmanTree);               // 释放哈夫曼树空间

powStr* BulidHufmanTreeOneTime(u8* file,u32 length);    // 一次性读完，建立哈夫曼树

#define _TabHufmanCreat(hufmanTree,map)     TabHufmanCreat(hufmanTree,map,&tabhufmanstaticbuff)     // // 解析树，保存映射表

mapTabStr* BulidHufmanTabForStr(u8* str,u32 length) ;              //一次性读完字符串str，建立映射表

mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* datbuf);      // 压缩

// 解压，返回原来的字节
u8 UncopressByte( mapTabStr* map,u8* hufmanfile,u32* datbuf_byte,u8* datbuf_bit);
u32 StrGetLength(u8* str);

#endif // !__HUFMAN_H
