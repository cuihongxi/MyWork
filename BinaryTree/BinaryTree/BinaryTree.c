#include "BinaryTree.h"

/**
 * �½�һ��������Ҷ�ӽڵ�
 */
btreeStr* NewBinaryTreeNode(void)
{
    btreeStr* bt = (btreeStr*)malloc(sizeof(btreeStr));
    bt->right = 0;
    bt->left = 0;
	return bt;
}



/**
 * �ͷŽڵ�ռ� 
 */
void FreeBinaryNode(btreeStr* bs)
{
    free(bs);
}