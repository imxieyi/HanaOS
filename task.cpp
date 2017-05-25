//Reference: http://wiki.osdev.org/Kernel_Multitasking
#include "task.hpp"
#include "heap.hpp"
#include "asmfunc.hpp"

#define TASK_STACK_SIZE 64*1024

TASKCTRL *taskctl;
TIMER *mt_timer;

extern MEMMAN *memman;

Task *initTasking(TIMER *timer){
	taskctl=(TASKCTRL*)memman->alloc_4k(sizeof(TASKCTRL));
	for(int i=0;i<MAX_TASKS;i++)
		taskctl->tasks0[i].stat=EMPTY;
	auto task=taskctl->alloc();
	task->stat=RUNNING;
	taskctl->running=1;
	taskctl->now=0;
	task->priority=2;
	taskctl->tasks[0]=task;
	asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(task->regs.cr3)::"%eax");
	asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(task->regs.eflags)::"%eax");
	
	//Timer
	mt_timer=timer;
	mt_timer->set(task->priority);
	return task;
}

void task_run(Task *task,int priority){
	if(priority!=0)
		task->priority=priority;
	if(task->stat!=RUNNING){
		task->stat=RUNNING;
		taskctl->tasks[taskctl->running]=task;
		taskctl->running++;
	}
}

Task *TASKCTRL::alloc(){
	for(int i=0;i<MAX_TASKS;i++)
		if(taskctl->tasks0[i].stat==EMPTY){
			auto task=&taskctl->tasks0[i];
			task->stat=SLEEPING;
			task->regs.eax=0;
			task->regs.ebp=0;
			task->regs.ecx=0;
			task->regs.edx=0;
			task->regs.esi=0;
			task->regs.edi=0;
			task->regs.eflags=taskctl->tasks0[0].regs.eflags;
			task->regs.cr3=taskctl->tasks0[0].regs.cr3;
			task->regs.esp=memman->alloc_4k(TASK_STACK_SIZE)+TASK_STACK_SIZE;
			return task;
		}
	return 0;
}

void mt_taskswitch(){
	auto prev=taskctl->tasks[taskctl->now];
	if(taskctl->running>=2){
		taskctl->now++;
		if(taskctl->now==taskctl->running)
			taskctl->now=0;
		auto task=taskctl->tasks[taskctl->now];
		mt_timer->set(task->priority);
		switchTask(&prev->regs,&task->regs);
	}else{
		mt_timer->set(prev->priority);
	}
}

Task *createTask(void (*main)()){
	auto task=taskctl->alloc();
	task->regs.eip=(uintptr_t)main;
	return task;
}

void sleepTask(){
	auto task=taskctl->tasks[taskctl->now];
	int i;
	for(i=0;i<taskctl->running;i++)
		if(taskctl->tasks[i]==task)
			break;
	taskctl->running--;
	for(;i<taskctl->running;i++)
		taskctl->tasks[i]=taskctl->tasks[i+1];
	task->stat=SLEEPING;
	if(taskctl->now>=taskctl->running)
		taskctl->now=0;
		switchTask(&task->regs,&taskctl->tasks[taskctl->now]->regs);
}

void exitTask(){
	auto task=taskctl->tasks[taskctl->now];
	for(int i=taskctl->now;i<taskctl->running;i++)
		taskctl->tasks[i]=taskctl->tasks[i+1];
	taskctl->running--;
	memman->free_4k(task->regs.esp,TASK_STACK_SIZE);
	task->stat=EMPTY;
	if(taskctl->now>=taskctl->running)
		taskctl->now=0;
	switchTask(&task->regs,&taskctl->tasks[taskctl->now]->regs);
}
