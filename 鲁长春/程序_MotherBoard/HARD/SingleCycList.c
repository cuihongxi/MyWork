#include "UHEAD.H"
#include "SingleCycList.h"


//创建一个循环链表

SingleCycList* SingleCycList_Create(void)
{
	SingleCycListNodeStr* cyc = (SingleCycListNodeStr*)malloc(sizeof(SingleCycListNodeStr));
	cyc->next = 0;
	return cyc;
}
///循环单链表插入
int SingleCycList_Insert(SingleCycList* list, SingleCycListNode* node) // O(n)
{ 
    SingleCycListNodeStr* current = (SingleCycListNodeStr*)list;
	
    if(current->next != 0) 
	{
		current = current->next;
		while(current->next != ((SingleCycListNodeStr*)list)->next)
		{
			current = current->next;
		}

	}
	current->next = (SingleCycListNodeStr*)node;
	((SingleCycListNodeStr*)node)->next = ((SingleCycListNodeStr*)list)->next;
    return 0;
}

//返回移除节点的上一个节点
//使用的前提是这个节点在这个链表中
SingleCycListNode*  SingleCycList_DeleteNode(SingleCycList* list, SingleCycListNode* node)
{
    SingleCycListNodeStr* current = (SingleCycListNodeStr*)list;
	
	if(current->next == 0) return 0;
	SingleCycListNodeStr* cc = current->next;	//记录入口的第一个节点
    while(current->next != (SingleCycListNodeStr*)node)
    {          
        current = current->next;
		if(current->next == cc) 
		{
//			debug("寻找了一圈没找到\r\n");
			return 0; //寻找了一圈没找到
		}
    } 
	if(current->next == current->next->next) 	//说明只有一个节点
	{
		current->next->next = 0;
		current->next = 0;
//		debug("\r\n没有节点了,list = %lu  ",(u32)list);
		
	}else
	{
		if(current->next == cc)				//要删除的节点是入口的第一个节点
		{
//			debug("多个节点，要删除的节点是入口的第一个节点\r\n");
			current->next = ((SingleCycListNodeStr*)node)->next;
			cc = current->next;
			while(current->next != (SingleCycListNodeStr*)node)
			{
				 current = current->next;
			}
			current->next = cc;
		}else
		{
//			debug("删除节点\r\n");
			current->next = ((SingleCycListNodeStr*)node)->next;
		}

	}
    return (SingleCycListNode*)list;
}

//迭代器
//如果有下一个则返回下一个指针，否则返回 0
SingleCycListNode* SingleCycList_Iterator(SingleCycListNode** node)
{	
	if(*node)	*node = ((SingleCycListNodeStr*)*node)->next;
	return (SingleCycListNode*)*node;
}


