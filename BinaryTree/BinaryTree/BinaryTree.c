#include "BinaryTree.h"

/**
 * 新建一个二叉树叶子节点
 */
btreeStr* NewBinaryTreeNode(void)
{
    btreeStr* bt = (btreeStr*)malloc(sizeof(btreeStr));
    bt->right = 0;
    bt->left = 0;
	return bt;
}



/**
 * 释放节点空间 
 */
void FreeBinaryNode(btreeStr* bs)
{
    free(bs);
}