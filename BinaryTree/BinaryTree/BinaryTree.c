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

/**
 *  前序遍历执行的操作，根据具体情况修改
 */

void Binaryvisit(btreeStr* bt)
{
    if(bt->left == 0 && bt->right == 0)
    {

    }
}
/**
 *  前序遍历 
 */
void BinaryPerTravers(btreeStr* bt)
{
    if (bt)
    {
        Binaryvisit(bt);
        BinaryPerTravers(bt->left);
        BinaryPerTravers(bt->right);
    }
    
}