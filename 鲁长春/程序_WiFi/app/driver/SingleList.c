
#include "SingleList.h"
#include "malloc.h"


// ��������
SingleList* ICACHE_FLASH_ATTR  NewSingleList(void)
{
	 SingleList* sl = (SingleList*)malloc(sizeof(SingleList));
	 sl->next = 0;
	 sl->num = 0;
	 return sl;
}

// ����β������ڵ�,�����²���Ľڵ��ַ
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

//ɾ���ڵ㣬���ͷſռ䣬�����Ƴ��ڵ����һ���ڵ�λ��
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

//������
//�������һ���򷵻���һ��ָ�룬���򷵻� 0
SingleListNode* ICACHE_FLASH_ATTR SingleList_Iterator(SingleListNode** node)
{
	if(*node) *node = ((SingleListNodeStr*)*node)->next;
	return ((SingleListNodeStr*)*node);

}

// �ͷ�����
void ICACHE_FLASH_ATTR FreeSingList(SingleList* list)
{

	SingleListNode* node = (SingleListNode*)list;
	while(SingleList_Iterator(&node))
	{
		 SingleList_DeleteNode(list,node);
	}
	free(list);
}

//���ڵ��Ƶ������
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

