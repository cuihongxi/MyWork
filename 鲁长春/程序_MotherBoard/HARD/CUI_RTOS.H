#ifndef __CUI_RTOS_H
#define __CUI_RTOS_H
#include "uhead.h"
#include "SingleList.h"

typedef struct{
	SingleListNodeStr list;			//链表
}TimerLinkStr;

typedef void(*osfun)(void) ;

typedef struct{
	SingleListNodeStr list;			//链表
	void(*osfun)(void);				//任务
	u32 time;						//定时时间
}funLinkStr;

typedef	struct{
	SingleListNodeStr timerlist;	//链表,用于添加到定时器队列中
	SingleListNodeStr funlink;		//链表,用于添加到任务队列中
	u8 flag_run;					//是否阻塞标志
	u8 step;						//步骤
	TimerLinkStr* timer;			//定时器执行链表
	u32 counter;					//定时器计数，ms
	u32 time;						//定时时间
		
}TaskStr;


void OS_TimerFunc(TimerLinkStr* timer);							// 定时器内函数
TaskStr* OS_CreatTask(TimerLinkStr* timer);						// 创建一个任务，绑定定时器
void OS_AddFunction(TaskStr* task,osfun fun,u32 time);			// 添加函数到任务,有顺序
void OsSectionFun(TaskStr* task);								// 运行实例函数

#endif
