/**
	���չ�Ƕ�γ̴����ݽṹ�ı�
	��������ļ��Ľṹ����ǰ�������SingleListNodeStr����
*/

#ifndef _SingleList_H_
#define _SingleList_H_

typedef void SingleList;
typedef void SingleListNode;

typedef struct _tag_SingleListNode SingleListNodeStr;

struct _tag_SingleListNode
{
    SingleListNodeStr* next;

};

int SingleList_Insert(SingleList* list, SingleListNode* node);

SingleListNode* SingleList_DeleteNode(SingleList* list, SingleListNode* node);

SingleListNode* SingleList_Iterator(SingleListNode** node);//������,�������һ���򷵻���һ��ָ�룬���򷵻� 0


#endif
