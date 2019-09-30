#include "CUI_RTOS.H"
#include "CUI_MALLOC1.H"
#include "CUI_MALLOC0.H"

//获取系统时间
u32 GetSysTime(TimerLinkStr* timerlink)
{
	return timerlink->counter * IRQ_PERIOD;
}

//释放内存
void OS_FreeRom(SingleList* list,void * prom)
{
	SingleList_Insert(list,prom);	// 添加到内存释放空间
	//free(prom);
}
//删除当前任务的节点之前所有任务，包括该任务
//如果下面还有任务则返回真
bool Free_taskBefore(TaskStr* task)
{
	funLinkStr* that = (funLinkStr*)&task->funNode;
	
	if(SingleList_Iterator((void**)&that) !=0 )// 如果任务链表下有任务
	{
		funLinkStr* pcurrent = that;
		if( task->pthis != that)
		{
			that = SingleList_DeleteNode(&task->funNode,pcurrent); 		// 删除节点
			OS_FreeRom(&(task->timerNode.tasklink->ramlink),pcurrent);	// 添加到内存释放空间
			while(SingleList_Iterator((void**)&that) && task->pthis != that) //先删除节点，后释放空间
			{	
				pcurrent = that;												// 保存该节点地址
				that = SingleList_DeleteNode(&task->funNode,pcurrent); 		// 删除节点
				OS_FreeRom(&(task->timerNode.tasklink->ramlink),pcurrent);	// 添加到内存释放空间
			}
				
		}
		//执行pthis == that操作
		pcurrent = that;
		if(SingleList_Iterator((void**)&that))
		{
			task->state = Wait;
		}else 
		{
			SingleCycList_DeleteNode(task->timerNode.tasklink, &task->taskNode);// 将该任务从任务循环队列中移除
			task->state = Stop;
		}
		SingleList_DeleteNode(&task->funNode,pcurrent); 			// 删除节点		
		OS_FreeRom(&(task->timerNode.tasklink->ramlink),pcurrent);		// 添加到内存释放空间
	}

	if(task->state == Stop) return (bool)FALSE;
	else return (bool)TRUE ;
	
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
			task->timerNode.counter = GetSysTime(task->timerlink) + time;		// 计时阀值
			SingleList_Insert(task->timerlink, &task->timerNode);			// 添加到定时任务
			SingleCycList_DeleteNode(task->timerNode.tasklink, &task->taskNode);	// 将该任务从任务循环队列中移除
		}		
		else	//释放该任务的动态函数空间
		{
				Free_taskBefore(task);
				task->pthis = 0;
		}	
	}else
	{
		if(task->pthis->number)
		{
			task->pthis->number --;
			task->state = Wait;
			
		}else Free_taskBefore(task);
		
		task->pthis = 0;
				
	}
}

//创建一个任务，绑定定时器
TaskStr* OS_CreatTask(TimerLinkStr* timerlink)
{
	TaskStr* task = (TaskStr*)mallocTask(sizeof(TaskStr));
	if(task == 0)
	{
		debug("OS_CreatTask :malloc NULL \r\n");
		return 0;
	}
	task->pthis = 0;
	task->timerlink = timerlink;
	task->timerNode.task = task;
	task->state = Stop;
	return task;
}

//添加函数到任务
void OS_AddFunction(TaskStr* task,osfun fun,u32 time)
{
	funLinkStr* funNode = (funLinkStr*)mallocFun(sizeof(funLinkStr));
	if(funNode == 0)
	{
		debug("OS_AddFunction :malloc NULL \r\n");
		return;
	}
	funNode->osfun = fun;
	funNode->time = time;
	funNode->number = MAX_NUMBER;
	funNode->type = general;
	SingleList_Insert(&task->funNode, funNode);
}

//添加循环函数到任务
//将前面的函数执行num次之后，再将其删除，并继续往下执行
void OS_AddCycleFunction(TaskStr* task,TYPE_NUMBER num)
{
	funLinkStr* funNode = (funLinkStr*)mallocFun(sizeof(funLinkStr));
	if(funNode == 0)
	{
		debug("OS_AddCycleFunction :malloc NULL \r\n");
		return;
	}
	funNode->osfun = OS_DeleteTask;
	funNode->time = 0;
	funNode->number = num - 1;			// 因为前面已经执行一遍了
	funNode->type = general;
	SingleList_Insert(&task->funNode, funNode);
}

//添加条件判断函数到任务
//定时器中判断条件函数如果为真则计时结束，跳转任务
//主函数中如果条件为真则删除该任务前面的所有函数
void OS_AddJudegeFunction(TaskStr* task,osfun fun,u32 time,jugefun funJuge)
{
	judgeFunStr* funNode = (judgeFunStr*)mallocFun(sizeof(judgeFunStr));
	if(funNode == 0)
	{
		debug("OS_AddJudegeFunction :malloc NULL \r\n");
		return;
	}	
	funNode->funLink.osfun = fun;
	funNode->funLink.time = time;
	funNode->funLink.number = MAX_NUMBER;
	funNode->funLink.type = judge;
	funNode->jugefun = funJuge;
	funNode->result = (bool)FALSE;
	SingleList_Insert(&task->funNode, funNode);
}

//添加任务到任务队列
void OS_AddTask(TaskLinkStr* tasklink, TaskStr* task)
{
	task->timerNode.tasklink = tasklink;
	task->state = Wait;
	SingleCycList_Insert(&tasklink->tasklink, &task->taskNode);
}

//运行实例函数
void OsSectionFun(TaskStr* task)
{
	if(task->pthis == 0) task->pthis = (funLinkStr*)&task->funNode;
	if(task->pthis->type == judge && (((judgeFunStr*)(task->pthis))->result == TRUE))	//条件判断为真，删除该任务所有函数
	{
		Free_taskBefore(task);	
	}
	SingleList_Iterator((void**)&task->pthis);					// 取一个任务
	
	if(task->pthis != 0 && task->pthis->osfun !=0)
	{
		task->pthis->osfun();							// 执行函数
		CUI_RTOS_Delayms(task,task->pthis->time);				// 执行延时
		return;													// 跳出程序
	}		
}

//判断任务是否为空，是返回真，否则返回假
bool OsJudge_TaskIsNull(TaskStr* task)
{
	funLinkStr* pthis =  (funLinkStr*)&task->funNode;
	if(SingleList_Iterator((void**)&pthis) == 0)					//没有任务
	return (bool)TRUE;
	else return (bool)FALSE;
}

extern TimerLinkStr 		timer2;
//任务队列运行
void OS_Task_Run(TaskLinkStr* tasklink)
{
	SingleCycListNode* ptask = &tasklink->tasklink;
	while(SingleCycList_Iterator((SingleCycListNode**)&ptask))
	{
		((TaskStr*)ptask)->state = Run;
		OsSectionFun((TaskStr*)ptask);
	}
	//释放内存
 	ptask = &tasklink->ramlink;
#if SHOW_FREE_TIMEMS >0
	u32 time = GetSysTime(&timer2);
#endif
	if(SingleList_Iterator((SingleListNode**)&ptask))
	{
		SingleCycListNode* back = ptask;
		ptask = SingleList_DeleteNode(&tasklink->ramlink, ptask);
		freeFun(back);
#if SHOW_FREE_TIMEMS >0
		debug("free time = %lu ms\r\n",GetSysTime(&timer2) - time);
#endif
	}
	
		
	
}


//定时器内函数
//返回计时器值
u32 OS_TimerFunc(TimerLinkStr* timer)
{
	TimerLinkStr* pNext = timer;
	timer->counter += IRQ_PERIOD;
	while(SingleList_Iterator((void**)&pNext))
	{

		if(pNext->counter <= GetSysTime(timer))
		{
			OS_AddTask(pNext->tasklink,pNext->task) ;				// 添加任务到队列	
			SingleList_DeleteNode(timer, pNext);					// 删除定时

		}else
		{
			if(pNext->task->pthis->type == judge)					// 如果指向条件判断函数，则函数为真时跳转
			{
				if((((judgeFunStr*)(pNext->task->pthis))->jugefun()) != 0) 
				{
					((judgeFunStr*)(pNext->task->pthis))->result = (bool)TRUE;
					//if(Free_taskBefore(pNext->task)) 
						OS_AddTask(pNext->tasklink,pNext->task) ;	// 添加任务到队列	
					SingleList_DeleteNode(timer, pNext);			// 删除定时
					
				}
			}
		}
	}
	return GetSysTime(timer);
}

//从任务列表删除任务用，时间应该为0
void OS_DeleteTask(void)
{
	return;
}

