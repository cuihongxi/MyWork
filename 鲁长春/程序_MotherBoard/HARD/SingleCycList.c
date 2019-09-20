#include "UHEAD.H"
#include "SingleCycList.h"


//����һ��ѭ������

SingleCycList* SingleCycList_Create(void)
{
	SingleCycListNodeStr* cyc = (SingleCycListNodeStr*)malloc(sizeof(SingleCycListNodeStr));
	cyc->next = 0;
	return cyc;
}
///ѭ�����������
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

//�����Ƴ��ڵ����һ���ڵ�
//ʹ�õ�ǰ��������ڵ������������
SingleCycListNode*  SingleCycList_DeleteNode(SingleCycList* list, SingleCycListNode* node)
{
    SingleCycListNodeStr* current = (SingleCycListNodeStr*)list;
	
	if(current->next == 0) return 0;
	SingleCycListNodeStr* cc = current->next;	//��¼��ڵĵ�һ���ڵ�
    while(current->next != (SingleCycListNodeStr*)node)
    {          
        current = current->next;
		if(current->next == cc) 
		{
//			debug("Ѱ����һȦû�ҵ�\r\n");
			return 0; //Ѱ����һȦû�ҵ�
		}
    } 
	if(current->next == current->next->next) 	//˵��ֻ��һ���ڵ�
	{
		current->next->next = 0;
		current->next = 0;
//		debug("\r\nû�нڵ���,list = %lu  ",(u32)list);
		
	}else
	{
		if(current->next == cc)				//Ҫɾ���Ľڵ�����ڵĵ�һ���ڵ�
		{
//			debug("����ڵ㣬Ҫɾ���Ľڵ�����ڵĵ�һ���ڵ�\r\n");
			current->next = ((SingleCycListNodeStr*)node)->next;
			cc = current->next;
			while(current->next != (SingleCycListNodeStr*)node)
			{
				 current = current->next;
			}
			current->next = cc;
		}else
		{
//			debug("ɾ���ڵ�\r\n");
			current->next = ((SingleCycListNodeStr*)node)->next;
		}

	}
    return (SingleCycListNode*)list;
}

//������
//�������һ���򷵻���һ��ָ�룬���򷵻� 0
SingleCycListNode* SingleCycList_Iterator(SingleCycListNode** node)
{	
	if(*node)	*node = ((SingleCycListNodeStr*)*node)->next;
	return (SingleCycListNode*)*node;
}


