#include "CUI_RTOS.H"

//获取系统时间
u32 GetSysTime(TimerLinkStr* timerlink)
{
	return timerlink->counter;
}
//系统延时函数
// task：任务 ；time 延时的值
void CUI_RTOS_Delayms(TaskStr* task,u32 time)
{
	if(task->pthis->number == MAX_NUMBER)
	{
		if(time)
		{
			task->state = Suspend;
			task->timerNode.counter = GetSysTime(task->timerlink) + time;						//计时阀值
			SingleList_Insert(task->timerlink, &task->timerNode);							// 添加到定时任务
			SingleCycList_DeleteNode(task->timerNode.tasklink, &task->taskNode);			// 将该任务从任务循环队列中移除
		}		
		else	//释放该任务的动态函数空间
		{
			funLinkStr* that = (funLinkStr*)&task->funNode;
			while(SingleList_Iterator((void**)&that) && task->pthis != that)
			{
				free(that);										// 释放空间
				SingleList_DeleteNode(&task->funNode,that); 	// 删除节点
			}
			if(SingleList_Iterator((void**)&task->pthis))
			{
				task->state = Wait;
		//		debug("还有任务\r\n");
			}else 
			{
				task->state = Stop;
				SingleCycList_DeleteNode(task->timerNode.tasklink, &task->taskNode);			// 将该任务从任务循环队列中移除
			}
			free(that);										// 释放空间
			SingleList_DeleteNode(&task->funNode,that); 	// 删除节点
			task->pthis = 0;
		//	debug("free ok\r\n");
			
		}	
	}else
	{
		if(task->pthis->number)
		{
			task->pthis->number --;
			task->state = Wait;
			
		}else
		{
			funLinkStr* that = (funLinkStr*)&task->funNode;
			while(SingleList_Iterator((void**)&that) && task->pthis != that)
			{
				free(that);										// 释放空间
				SingleList_DeleteNode(&task->funNode,that); 	// 删除节点
			}
			if(SingleList_Iterator((void**)&task->pthis))
			{
				task->state = Wait;
			//	debug("还有任务\r\n");
			}else 
			{
				task->state = Stop;
				SingleCycList_DeleteNode(task->timerNode.tasklink, &task->taskNode);			// 将该任务从任务循环队列中移除
			}
			free(that);										// 释放空间
			SingleList_DeleteNode(&task->funNode,that); 	// 删除节点
		//	debug("free ok\r\n");
		}
		task->pthis = 0;
		
	}

	


	
}




//创建一个任务，绑定定时器
TaskStr* OS_CreatTask(TimerLinkStr* timerlink)
{
	TaskStr* task = (TaskStr*)malloc(sizeof(TaskStr));
	task->pthis = 0;
	task->timerlink = timerlink;
	task->timerNode.task = task;
	task->state = Stop;
	return task;
}

//添加函数到任务
void OS_AddFunction(TaskStr* task,osfun fun,u32 time)
{
	funLinkStr* funNode = (funLinkStr*)malloc(sizeof(funLinkStr));
	funNode->osfun = fun;
	funNode->time = time;
	funNode->number = MAX_NUMBER;
	SingleList_Insert(&task->funNode, funNode);
}

//添加循环函数到任务
void OS_AddCycleFunction(TaskStr* task,TYPE_NUMBER num)
{
	funLinkStr* funNode = (funLinkStr*)malloc(sizeof(funLinkStr));
	funNode->osfun = OS_DeleteTask;
	funNode->time = 0;
	funNode->number = num;
	SingleList_Insert(&task->funNode, funNode);
}

//添加任务到任务队列
void OS_AddTask(TaskLinkStr* tasklink, TaskStr* task)
{
	task->timerNode.tasklink = tasklink;
	task->state = Wait;
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
		((TaskStr*)tasklink)->state = Run;
		OsSectionFun((TaskStr*)tasklink);
	}
	
}


//定时器内函数
void OS_TimerFunc(TimerLinkStr* timer)
{
	TimerLinkStr* pNext = timer;
	timer->counter ++;
	while(SingleList_Iterator((void**)&pNext))
	{
		if(pNext->counter == timer->counter)
		{
			OS_AddTask(pNext->tasklink,pNext->task) ;			// 添加任务到队列	
			SingleList_DeleteNode(timer, pNext);							// 删除定时

		}
		
	}
}

//从任务列表删除任务用，时间应该为0
void OS_DeleteTask(void)
{
	return;
}

