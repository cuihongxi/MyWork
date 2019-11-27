
#include "SingleList.h"
#include "malloc.h"


// 创建链表
SingleList* ICACHE_FLASH_ATTR  NewSingleList(void)
{
	 SingleList* sl = (SingleList*)malloc(sizeof(SingleList));
	 sl->next = 0;
	 sl->num = 0;
	 return sl;
}

// 链表尾部插入节点,返回新插入的节点地址
unsigned int ICACHE_FLASH_ATTR SingleList_InsertEnd(SingleList* list, SingleListNode* node)
{ 
    SingleList* current = list;

    SingleListNodeStr* newnode = (SingleListNodeStr*)malloc(sizeof(SingleListNodeStr));
	newnode->next = 0;
	newnode->node = node;

    for(; current->next != 0;)
    {
        current = current->next;
    }
	current->next = newnode;     
    list->num ++;
    return (unsigned int)newnode;
}

//删掉节点，并释放空间，返回移除节点的上一个节点位置
SingleListNode* ICACHE_FLASH_ATTR SingleList_DeleteNode(SingleList* list, SingleListNode* node)
{

    SingleList* current = list;
	void* back;
    for(;current->next != 0; )
    {
        if( ((SingleListNodeStr*)(current->next))->node == node ) break;
        current = current->next;
    }
	if(current->next == 0) return (SingleListNode*)current;
	back =  current->next;
    current->next = ((SingleListNodeStr*)(current->next))->next;
	free(back);
	list->num --;
    return (SingleListNode*)current;
}

//迭代器
//如果有下一个则返回下一个指针，否则返回 0
SingleListNode* ICACHE_FLASH_ATTR SingleList_Iterator(SingleListNode** node)
{
	if(*node) *node = ((SingleListNodeStr*)*node)->next;
	return ((SingleListNodeStr*)*node);

}

// 释放链表
void ICACHE_FLASH_ATTR FreeSingList(SingleList* list)
{

	SingleListNode* node = (SingleListNode*)list;
	while(SingleList_Iterator(&node))
	{
		 SingleList_DeleteNode(list,node);
	}
	free(list);
}

//将节点移到最后面
void ICACHE_FLASH_ATTR SingleList_MoveEndNode(SingleList* list, SingleListNode* node)
{

    SingleList* current = list;
	void* back;
    for(;current->next != 0; )
    {
        if( ((SingleListNodeStr*)(current->next))->node == node )
        	{

        		break;
        	}
        current = current->next;

    }
    if(current->next == 0)return;
	if(((SingleListNodeStr*)(current->next))->next)
	{
		back = current->next;

		current->next = ((SingleListNodeStr*)(current->next))->next;
	    for(; current->next != 0;)
	    {
	        current = current->next;
	    }
	    current->next = back;
	    ((SingleList*)back)->next = 0;
	}
}

