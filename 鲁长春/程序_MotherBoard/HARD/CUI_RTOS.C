#include "CUI_RTOS.H"

//系统延时函数
// task：任务 ；time 延时的值
void CUI_RTOS_Delayms(TaskStr* task,u32 time)
{
	task->flag_run = FALSE;					//防止重入
	task->time = time;						//计时阀值
	task->counter = 0;						//计时归零
	SingleList_Insert(task->timer, task);	//添加到定时任务

}

//创建一个任务，绑定定时器
TaskStr* OS_CreatTask(TimerLinkStr* timer)
{
	TaskStr* task = (TaskStr*)malloc(sizeof(TaskStr));

	task->timer = timer;
	task->step = 0;
	task->flag_run = TRUE;
	return task;
}

void LED_RED_ON(void);
//添加函数到任务
void OS_AddFunction(TaskStr* task,osfun fun,u32 time)
{
	funLinkStr* funlink = (funLinkStr*)malloc(sizeof(funLinkStr));
	funlink->osfun = fun;
	funlink->time = time;
	SingleList_Insert(&task->funlink, funlink);

}

//运行实例函数
void OsSectionFun(TaskStr* task)
{
	
	if(task->flag_run == TRUE)
	{
		funLinkStr* phasnext = (funLinkStr*)&task->funlink;
		SingleList_Iterator((void**)&phasnext);
		u8 step = task->step;
		while(step--)  SingleList_Iterator((void**)&phasnext);	// 根据step，跳转到执行函数中
		if(phasnext != 0)
		{
			phasnext->osfun();									// 执行函数
			CUI_RTOS_Delayms(task,phasnext->time);				// 执行延时
			return;									//跳出程序

		}
		task->step = 0;
	}
}

//定时器内函数
void OS_TimerFunc(TimerLinkStr* timer)
{
	TaskStr* pNext = (TaskStr*)timer;
	
	while(SingleList_Iterator((void**)&pNext))
	{
		pNext->counter ++;
		//debug("pNext->counter = %lu ,pNext->time = %lu\r\n,",pNext->counter,pNext->time);
		if(pNext->counter == pNext->time)
		{
			pNext->step ++;
			pNext->flag_run = TRUE;
			SingleList_DeleteNode(timer, pNext);
			
		}
		
	}
}
