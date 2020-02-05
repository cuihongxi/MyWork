/*
	2020年2月4日15:35:06
    二叉树
*/

#ifndef		__BinaryTree_H
#define		__BinaryTree_H

#include "uhead.h"

typedef struct{
    void*	left;
    void*   right;
}btreeStr;



/* 函数 */

btreeStr* NewBinaryTreeNode(void);              // 新建一个二叉树叶子节点
void FreeBinaryNode(btreeStr* bs);

#endif