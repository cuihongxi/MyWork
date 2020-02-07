/**
 * 2020年2月4日16:33:43
 * 霍夫曼算法压缩及解压缩程序
 * 备注：   程序使用了动态内存分配和单链表的数据结构，需要相应的文件
 * 压缩步骤：
 * ①    创建一个哈夫曼映射表变量：    mapTabStr* map;
 *              注：该变量保存了映射关系，哈夫曼树地址，被压缩文件大小。哈夫曼树在压缩之后和解压时没有被调用，但仍然被保留，
 *                如果不保留可以调用 Free_HufmanTree(powStr* hufmanTree);释放空间
 * ②    调用压缩文件函数：  mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* hufmanfile);
 *              注：对file文件进行压缩，压缩之后的编码保存在 hufmanfile 中。同时返回映射表变量用map接收。
 *              例子：map = HufmanCompressFile("111111222233344556",19,hufmanfile); 
 * 解压步骤：
 * ①    调用解压缩函数： void HufmanUncompressFile(u8* hufmanfile,mapTabStr* map,u8* datbuf);
 *              注：使用相应的哈夫曼映射表map ，解压压缩文件hufmanfile，解压缩之后的数据保存到datbuf中
 *              例子：HufmanUncompressFile(hufmanfile,map,encodefile);
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

mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* hufmanfile);      // 压缩
void HufmanUncompressFile(u8* hufmanfile,mapTabStr* map,u8* datbuf);// 解压

u32 StrGetLength(u8* str);

#endif // !__HUFMAN_H
