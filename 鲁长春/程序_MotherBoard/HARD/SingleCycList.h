/*
	单向循环链表
	2019年8月23日17:34:32
	调用这个文件的结构体最前面必须是SingleCycListNodeStr
*/

#ifndef _SingleCycList_H_
#define _SingleCycList_H_

typedef void SingleCycList;
typedef void SingleCycListNode;

typedef struct _SingleCycListNode SingleCycListNodeStr;

struct _SingleCycListNode
{
    SingleCycListNodeStr* next;		//下一个节点地址
};


//SingleCycList* SingleCycList_Create(void);
int SingleCycList_Insert(SingleCycList* list, SingleCycListNode* node) ;	// 循环单链表插入
SingleCycListNode*  SingleCycList_DeleteNode(SingleCycList* list, SingleCycListNode* node);
SingleCycListNode* SingleCycList_Iterator(SingleCycListNode** node);//迭代器,如果有下一个则返回下一个指针，否则返回 0

#endif
