#ifndef __CUI_RTOS_H
#define __CUI_RTOS_H
#include "uhead.h"
#include "SingleList.h"
#include "SingleCycList.h"
/*
2019年8月27日07:30:34
增加了系统时钟概念
增加了任务状态判断
待增加空闲任务时释放内存功能

*/


#define		MAX_NUMBER		0XFF		//任务的最大循环次数，等于就无限循环
#define		TYPE_NUMBER		u8			//数据类型
//任务链表
typedef struct{
	SingleListNodeStr list;			//链表
}TaskLinkStr;

typedef struct _TaskStr 	TaskStr;

//定时器链表
typedef struct{
	SingleListNodeStr list;			//链表
	u32 counter;					//定时器计数，ms
	TaskLinkStr* 	tasklink;		//任务执行链表	
	TaskStr*  task;
	
}TimerLinkStr;

typedef void(*osfun)(void) ;

typedef struct{
	SingleListNodeStr 	list;			//链表
	void(*osfun)(void);					//任务
	u32 				time;			//定时时间
	TYPE_NUMBER			number;			// 循环次数	，如果为最大值就无限循环，否则到达次数就被移除	
}funLinkStr;

typedef enum{
	Stop,
	Run,
	Wait,
	Suspend,						//挂起，等待定时器计时完成
}taskState;

//任务结构体
struct _TaskStr{
	SingleListNodeStr 	taskNode;		// 任务链接节点
	TimerLinkStr	 	timerNode;		// 定时器链接节点
	SingleListNodeStr 	funNode;		// 链表,用于添加到任务队列中
	funLinkStr* 		pthis;			// 记录当前执行到的函数
	TimerLinkStr* 		timerlink;		// 定时器执行链表
	taskState 			state;			// 任务状态
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
void OS_AddCycleFunction(TaskStr* task,TYPE_NUMBER num);		// 添加循环函数到任务
void OS_Task_Run(TaskLinkStr* tasklink);						// 任务队列运行

u32 GetSysTime(TimerLinkStr* timerlink);						// 获取系统时间
void OS_DeleteTask(void);										//从任务列表删除任务用，时间应该为0
#endif
