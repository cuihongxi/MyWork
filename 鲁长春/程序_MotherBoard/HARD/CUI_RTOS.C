#include "CUI_RTOS.H"

//系统延时函数
// task：任务 ；time 延时的值
void CUI_RTOS_Delayms(TaskStr* task,u32 time)
{
	task->timerNode.time = time;							//计时阀值
	task->timerNode.counter = 0;							//计时归零
//	debug("挂起任务 task ADDR = %lu, tasklink addr = %lu,time = %lu\r\n",(u32)&task->taskNode,(u32)task->timerlink->tasklink,time);
	SingleList_Insert(task->timerlink, &task->timerNode);					// 添加到定时任务
	SingleCycList_DeleteNode(task->timerNode.tasklink, &task->taskNode);	// 将该任务从任务循环队列中移除
	
}

//创建一个任务，绑定定时器
TaskStr* OS_CreatTask(TimerLinkStr* timerlink)
{
	TaskStr* task = (TaskStr*)malloc(sizeof(TaskStr));
	task->pthis = 0;
	task->timerlink = timerlink;
	task->timerNode.task = task;
	return task;
}

//添加函数到任务
void OS_AddFunction(TaskStr* task,osfun fun,u32 time)
{
	funLinkStr* funNode = (funLinkStr*)malloc(sizeof(funLinkStr));
	funNode->osfun = fun;
	funNode->time = time;
	SingleList_Insert(&task->funNode, funNode);
}

//添加任务到任务队列
void OS_AddTask(TaskLinkStr* tasklink, TaskStr* task)
{
	task->timerNode.tasklink = tasklink;
	SingleCycList_Insert(tasklink, &task->taskNode);
}

//运行实例函数
void OsSectionFun(TaskStr* task)
{
	if(task->pthis == 0) task->pthis = (funLinkStr*)&task->funNode;
	SingleList_Iterator((void**)&task->pthis);					//取一个任务
	if(task->pthis != 0 && task->pthis->osfun !=0)
	{
		task->pthis->osfun();									// 执行函数
		CUI_RTOS_Delayms(task,task->pthis->time);				// 执行延时
		return;													//跳出程序
	}	
}

//任务队列运行
void OS_Task_Run(TaskLinkStr* tasklink)
{
	while(SingleCycList_Iterator((SingleCycListNode**)&tasklink))
	{
		OsSectionFun((TaskStr*)tasklink);
	}
	
}

//定时器内函数
void OS_TimerFunc(TimerLinkStr* timer)
{
	TimerLinkStr* pNext = timer;
	while(SingleList_Iterator((void**)&pNext))
	{
		pNext->counter ++;
		if(pNext->counter == pNext->time)
		{
			SingleCycList_Insert(pNext->tasklink,&pNext->task->taskNode) ;	// 添加任务到队列	
			SingleList_DeleteNode(timer, pNext);							// 删除定时

		}
		
	}
}
