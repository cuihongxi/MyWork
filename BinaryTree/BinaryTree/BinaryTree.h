/*
	2020��2��4��15:35:06
    ������
*/

#ifndef		__BinaryTree_H
#define		__BinaryTree_H

#include "uhead.h"

typedef struct{
    void*	left;
    void*   right;
}btreeStr;



/* ���� */

btreeStr* NewBinaryTreeNode(void);              // �½�һ��������Ҷ�ӽڵ�
void FreeBinaryNode(btreeStr* bs);

#endif