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
2019年8月31日09:24:39
	增加条件判断函数，该函数如果条件为真则跳出延时，并删除之前的任务队列
		
*/

#define 	SHOW_FREE_TIMEMS 0		//串口打印释放内存的时间

#define		MAX_NUMBER		0XFF		// 任务的最大循环次数，等于就无限循环
#define		TYPE_NUMBER		u8			// 数据类型
#define		IRQ_PERIOD		1			// ms

//任务链表
typedef struct{
	SingleListNodeStr tasklink;				// 任务链表
	SingleListNodeStr ramlink;				// 内存释放链表
}TaskLinkStr;

typedef struct _TaskStr 	TaskStr;

//定时器链表
typedef struct{
	SingleListNodeStr list;			//链表
	u32 counter;					//定时器计数，ms
	TaskLinkStr* 	tasklink;		//任务执行链表	
	TaskStr*  		task;
	
}TimerLinkStr;

typedef void(*osfun)(void) ;

typedef enum{
	general,							// 一般 
	judge,								// 判断类型
}typefun;

// 通用函数结构体
typedef struct{
	SingleListNodeStr 	list;			// 链表
	void(*osfun)(void);					// 任务
	u32 				time;			// 定时时间
	TYPE_NUMBER			number;			// 循环次数	，如果为最大值就无限循环，否则到达次数就被移除
	typefun 			type;			// 任务类型 ，如果是judege则是条件判断函数结构体
	
}funLinkStr;

// 判断型函数结构体
typedef bool(*jugefun)(void);
typedef struct{
	funLinkStr funLink;
	bool(*jugefun)(void);
	bool result;						//判断函数的结果，为真时，则删除所有任务中的函数
}judgeFunStr;


// 任务运行状态
typedef enum{
	Stop,
	Run,
	Wait,
	Suspend,							// 挂起，等待定时器计时完成
}taskState;


//任务结构体
struct _TaskStr{
	SingleListNodeStr 	taskNode;		// 任务链接节点
	TimerLinkStr	 	timerNode;		// 定时器链接节点
	SingleListNodeStr 	funNode;		// 链表,用于添加到任务队列中
	funLinkStr* 		pthis;			// 记录当前执行到的函数
	TimerLinkStr* 		timerlink;		// 定时器执行链表
	taskState 		state;			// 任务状态
};



#define	mallocFun(a)		CUI_MALLOC1_OneUnit();
#define	freeFun(a)		FreeCUI_MALLOC1(a)
#define	mallocTask(a)		CUI_MALLOC0(&malloc0Mesg,a);

#define	OS_ITCLOSE()		disableInterrupts();
#define	OS_ITOPEN()		enableInterrupts();

/*函数*/

u32 OS_TimerFunc(TimerLinkStr* timer);					// 定时器内函数
TaskStr* OS_CreatTask(TimerLinkStr* timerlink);				// 创建一个任务，绑定定时器
void OS_AddFunction(TaskStr* task,osfun fun,u32 time);			// 添加函数到任务,有顺序
void OsSectionFun(TaskStr* task);					// 运行实例函数
void OS_AddTask(TaskLinkStr* tasklink, TaskStr* task);			// 添加任务到任务队列
void OS_AddCycleFunction(TaskStr* task,TYPE_NUMBER num);		// 添加循环函数到任务
void OS_Task_Run(TaskLinkStr* tasklink);				// 任务队列运行
u32 GetSysTime(TimerLinkStr* timerlink);				// 获取系统时间
void OS_DeleteTask(void);						// 从任务列表删除任务用，时间应该为0
void OS_AddJudegeFunction(TaskStr* task,osfun fun,u32 time,jugefun funJuge);// 添加条件判断函数到任务\
																定时器中判断条件函数如果为真则计时结束，跳转任务

bool OsJudge_TaskIsNull(TaskStr* task);					// 判断任务是否为空，是返回真，否则返回假

#endif
