#ifndef __CUI_RTOS_H
#define __CUI_RTOS_H
#include "uhead.h"
#include "SingleList.h"
#include "SingleCycList.h"

//任务链表
typedef struct{
	SingleListNodeStr list;			//链表
}TaskLinkStr;

typedef struct _TaskStr 	TaskStr;

//定时器链表
typedef struct{
	SingleListNodeStr list;			//链表
	u32 counter;					//定时器计数，ms
	u32 time;						//定时时间
	TaskLinkStr* 	tasklink;		//任务执行链表	
	TaskStr*  task;
}TimerLinkStr;

typedef void(*osfun)(void) ;

typedef struct{
	SingleListNodeStr list;			//链表
	void(*osfun)(void);				//任务
	u32 time;						//定时时间
}funLinkStr;

//任务结构体
struct _TaskStr{
	SingleListNodeStr 	taskNode;		//任务链接节点
	TimerLinkStr	 	timerNode;		//定时器链接节点
	SingleListNodeStr 	funNode;		//链表,用于添加到任务队列中
	funLinkStr* 		pthis;			//记录当前执行到的函数
	
	TimerLinkStr* 		timerlink;		//定时器执行链表
		
};

//任务链表节点
typedef struct {
	SingleListNodeStr taskNode;					//任务链接节点
	void(*fun)(TaskStr* task);					//任务
}TaskLinkStrNode;


void OS_TimerFunc(TimerLinkStr* timer);							// 定时器内函数
TaskStr* OS_CreatTask(TimerLinkStr* timerlink);					// 创建一个任务，绑定定时器
void OS_AddFunction(TaskStr* task,osfun fun,u32 time);			// 添加函数到任务,有顺序
void OsSectionFun(TaskStr* task);								// 运行实例函数

void OS_AddTask(TaskLinkStr* tasklink, TaskStr* task);			// 添加任务到任务队列
void OS_Task_Run(TaskLinkStr* tasklink);						// 任务队列运行
#endif
