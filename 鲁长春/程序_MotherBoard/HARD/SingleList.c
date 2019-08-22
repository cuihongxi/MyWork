
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

//返回移除节点的上一个节点，以保证->next相同
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

//迭代器
//如果有下一个则返回下一个指针，否则返回 0
SingleListNode* SingleList_Iterator(SingleListNode** node)
{
	*node = ((SingleListNodeStr*)*node)->next;
	return ((SingleListNodeStr*)*node);
}
