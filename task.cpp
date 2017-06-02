//Reference: http://wiki.osdev.org/Kernel_Multitasking
#include "task.hpp"
#include "heap.hpp"
#include "asmfunc.hpp"
#include "hanastd.hpp"
using namespace hanastd;

TASKCTRL *taskctl;
TIMER *mt_timer;

extern MEMMAN *memman;

void task_idle(void *arg){
	for(;;)io_hlt();
}

Task *task_now(){
	auto tl=&taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

void task_add(Task *task){
	auto tl=&taskctl->level[task->level];
	tl->tasks[tl->running]=task;
	tl->running++;
	task->stat=RUNNING;
}

void task_remove(Task *task){
	auto tl=&taskctl->level[task->level];
	int i;
	for(i=0;i<tl->running;i++)
		if(tl->tasks[i]==task)
			break;
	if(i==tl->running)
		return;
	tl->running--;
	if(i<tl->now)
		tl->now--;
	if(tl->now>=tl->running)
		tl->now=0;
	task->stat=SLEEPING;
	for(;i<tl->running;i++)
		tl->tasks[i]=tl->tasks[i+1];
}

void task_killremove(Task *task){
	auto tl=&taskctl->level[task->level];
	int i;
	if(task->stat==RUNNING){
		tl->running--;
		if(tl->tasks[tl->now]==task)
			tl->now=(tl->now+1)%tl->running;
	}
	for(i=0;tl->tasks[i]!=NULL;i++)
		if(tl->tasks[i]==task)
			break;
	for(;tl->tasks[i]!=NULL;i++)
		tl->tasks[i]=tl->tasks[i+1];
}

void task_switchsub(){
	int i;
	for(i=0;i<MAX_TASKLEVELS;i++)
		if(taskctl->level[i].running>0)
			break;
	taskctl->now_lv=i;
	taskctl->lv_change=0;
}

Task *initTasking(TIMER *timer){
	taskctl=(TASKCTRL*)memman->alloc_4k(sizeof(TASKCTRL));
	memset(taskctl,0,sizeof(TASKCTRL));
	for(int i=0;i<MAX_TASKS;i++)
		taskctl->tasks0[i].stat=EMPTY;
	auto task=taskctl->alloc();
	strncpy("kernel",task->name,6);
	task->stat=RUNNING;
	task->priority=2;
	task->level=0;
	task_add(task);
	task_switchsub();
	asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(task->regs.cr3)::"%eax");
	asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(task->regs.eflags)::"%eax");

	//Idle task
	auto idle=createTask("idle",&task_idle,NULL);
	task_run(idle,MAX_TASKLEVELS-1,1);
	
	//Timer
	mt_timer=timer;
	mt_timer->set(task->priority);
	return task;
}

void task_run(Task *task,int level,int priority){
	if(level<0)level=task->level;
	if(priority!=0)
		task->priority=priority;
	if(task->stat==RUNNING&&task->level!=level)
		task_remove(task);
	if(task->stat!=RUNNING){
		task->level=level;
		task_add(task);
	}
	taskctl->lv_change=1;
}

Task *TASKCTRL::alloc(){
	for(int i=0;i<MAX_TASKS;i++)
		if(taskctl->tasks0[i].stat==EMPTY){
			auto task=&taskctl->tasks0[i];
			memset(task,0,sizeof(Task));
			task->stat=SLEEPING;
			task->regs.eax=0;
			task->regs.ebp=0;
			task->regs.ecx=0;
			task->regs.edx=0;
			task->regs.esi=0;
			task->regs.edi=0;
			task->regs.eflags=taskctl->tasks0[0].regs.eflags;
			task->regs.cr3=taskctl->tasks0[0].regs.cr3;
			task->stackbottom=memman->alloc_4k(TASK_STACK_SIZE);
			task->regs.esp=task->stackbottom+TASK_STACK_SIZE-8;
			return task;
		}
	return 0;
}

void mt_taskswitch(){
	auto tl=&taskctl->level[taskctl->now_lv];
	Task *newtask,*nowtask=tl->tasks[tl->now];
	int i;
	for(i=tl->now+1;i!=tl->now;i=(i+1)%tl->running)
		if(tl->tasks[i]->stat==RUNNING)
			break;
	tl->now=i;
	if(taskctl->lv_change!=0){
		task_switchsub();
		tl=&taskctl->level[taskctl->now_lv];
	}
	newtask=tl->tasks[tl->now];
	mt_timer->set(newtask->priority);
	if(newtask!=nowtask)
		switchTask(&nowtask->regs,&newtask->regs);
}

Task *createTask(const char *name,void (*main)(void*),void *arg){
	auto task=taskctl->alloc();
	strncpy(name,task->name,strlen(name));
	task->regs.eip=(uintptr_t)main;
	*((uint32_t*)(task->regs.esp+4))=(uintptr_t)arg;
	return task;
}

void sleepTask(){
	auto task=task_now();
	task_remove(task);
	task_switchsub();
	auto now_task=task_now();
	switchTask(&task->regs,&now_task->regs);
}

void exitTask(){
	auto task=task_now();
	task_remove(task);
	memman->free_4k(task->regs.esp,TASK_STACK_SIZE);
	task->stat=EMPTY;
	task_switchsub();
	switchTask(&task->regs,&task_now()->regs);
}

void killTask(Task *task){
	auto tasknow=task_now();
	if(task==tasknow){
		exitTask();
	}else{
		task_remove(task);
		memman->free_4k(task->stackbottom,TASK_STACK_SIZE);
		task->stat=EMPTY;
	}
}
