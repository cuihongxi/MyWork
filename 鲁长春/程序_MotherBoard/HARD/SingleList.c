
#include "SingleList.h"

int SingleList_Insert(SingleList* list, SingleListNode* node) // O(n)
{ 
    int i = 0;
    SingleListNodeStr* current = (SingleListNodeStr*)list;
    
    for(i=0; current->next != 0; i++)
    {
        current = current->next;
    }
    
    ((SingleListNodeStr*)node)->next = 0;
    current->next = (SingleListNodeStr*)node;       
    
    return 0;
}

//�����Ƴ��ڵ����һ���ڵ㣬�Ա�֤->next��ͬ
SingleListNode*  SingleList_DeleteNode(SingleList* list, SingleListNode* node) // O(n)
{

    SingleListNodeStr* current = (SingleListNodeStr*)list;
    for(;current->next != 0; )
    {
        if( current->next == (SingleListNodeStr*)node ) break; 
                  
        current = current->next;
    }  
    current->next = ((SingleListNodeStr*)node)->next;

    return (SingleListNode*)current;
}

//������
//�������һ���򷵻���һ��ָ�룬���򷵻� 0
SingleListNode* SingleList_Iterator(SingleListNode** node)
{
	*node = ((SingleListNodeStr*)*node)->next;
	return ((SingleListNodeStr*)*node);
}
