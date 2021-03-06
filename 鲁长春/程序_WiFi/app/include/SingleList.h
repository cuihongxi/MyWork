/**
	依照国嵌课程大话数据结构改编
	调用这个文件的结构体最前面必须是SingleListNodeStr类型
*/

#ifndef _SingleList_H_
#define _SingleList_H_

#include "uhead.h"


typedef void SingleListNode;
typedef void listType;
typedef struct 
{
	listType* next;
    unsigned int num;				// 记录当前链表下的元素个数
}SingleList;


typedef struct 
{	

	listType* next;
	SingleListNode* node;

}SingleListNodeStr;


SingleList* NewSingleList(void);												// 创建链表
unsigned int SingleList_InsertEnd(SingleList* list, SingleListNode* node);		// 链表尾部插入节点
SingleListNode* SingleList_DeleteNode(SingleList* list, SingleListNode* node);	// 删掉节点，并释放空间，返回移除节点的上一个节点位置
SingleListNode* SingleList_Iterator(SingleListNode** node);						// 迭代器,如果有下一个则返回下一个指针，否则返回 0
void FreeSingList(SingleList* list);											// 释放链表
void SingleList_MoveEndNode(SingleList* list, SingleListNode* node);			// 将节点移到最后面
#define		SingeListGetnode(type,nodehead)		((type*)(((SingleListNodeStr*)nodehead)->node))	//获取链表下的节点

#endif
