/*
	����ѭ������
	2019��8��23��17:34:32
	��������ļ��Ľṹ����ǰ�������SingleCycListNodeStr
*/

#ifndef _SingleCycList_H_
#define _SingleCycList_H_

typedef void SingleCycList;
typedef void SingleCycListNode;

typedef struct _SingleCycListNode SingleCycListNodeStr;

struct _SingleCycListNode
{
    SingleCycListNodeStr* next;		//��һ���ڵ��ַ
};


//SingleCycList* SingleCycList_Create(void);
int SingleCycList_Insert(SingleCycList* list, SingleCycListNode* node) ;	// ѭ�����������
SingleCycListNode*  SingleCycList_DeleteNode(SingleCycList* list, SingleCycListNode* node);
SingleCycListNode* SingleCycList_Iterator(SingleCycListNode** node);//������,�������һ���򷵻���һ��ָ�룬���򷵻� 0

#endif
