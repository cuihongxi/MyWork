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

/**
 *  ǰ�����ִ�еĲ��������ݾ�������޸�
 */

void Binaryvisit(btreeStr* bt)
{
    if(bt->left == 0 && bt->right == 0)
    {

    }
}
/**
 *  ǰ����� 
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