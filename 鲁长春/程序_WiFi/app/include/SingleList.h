/**
	���չ�Ƕ�γ̴����ݽṹ�ı�
	��������ļ��Ľṹ����ǰ�������SingleListNodeStr����
*/

#ifndef _SingleList_H_
#define _SingleList_H_

#include "uhead.h"


typedef void SingleListNode;
typedef void listType;
typedef struct 
{
	listType* next;
    unsigned int num;				// ��¼��ǰ�����µ�Ԫ�ظ���
}SingleList;


typedef struct 
{	

	listType* next;
	SingleListNode* node;

}SingleListNodeStr;


SingleList* NewSingleList(void);											// ��������
unsigned int SingleList_InsertEnd(SingleList* list, SingleListNode* node);	// ����β������ڵ�
SingleListNode* SingleList_DeleteNode(SingleList* list, SingleListNode* node);
SingleListNode* SingleList_Iterator(SingleListNode** node);					// ������,�������һ���򷵻���һ��ָ�룬���򷵻� 0
void FreeSingList(SingleList* list);										// �ͷ�����

#define		SingeListGetnode(type,nodehead)		(*(type*)(((SingleListNodeStr*)nodehead)->node))

#endif
