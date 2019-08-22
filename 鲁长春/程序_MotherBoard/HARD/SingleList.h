/**
	依照国嵌课程大话数据结构改编
	调用这个文件的结构体最前面必须是SingleListNodeStr类型
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

SingleListNode* SingleList_Iterator(SingleListNode** node);//迭代器,如果有下一个则返回下一个指针，否则返回 0


#endif
