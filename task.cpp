//Reference: http://wiki.osdev.org/Kernel_Multitasking
#include "task.hpp"
#include "heap.hpp"
#include "asmfunc.hpp"

#define TASK_SWITCH_INTERVAL 2 //ms

static Task mainTask;
static Task *lastTask;
static Task *prevTask;
static Task *runningTask;

TIMER *mt_timer;

extern MEMMAN *memman;

void initTasking(TIMER *timer){
	asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
	asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
	
	mainTask.next=&mainTask;
	runningTask=&mainTask;
	lastTask=&mainTask;
	prevTask=&mainTask;
	
	//Timer
	mt_timer=timer;
	mt_timer->set(TASK_SWITCH_INTERVAL);
}

void mt_taskswitch(){
	mt_timer->set(TASK_SWITCH_INTERVAL);
	preempt();
}

void createTask(void (*main)()){
	auto task=(Task*)memman->alloc(sizeof(Task));
	task->regs.eax=0;
	task->regs.ebp=0;
	task->regs.ecx=0;
	task->regs.edx=0;
	task->regs.esi=0;
	task->regs.edi=0;
	task->regs.eflags=mainTask.regs.eflags;
	task->regs.eip=(uintptr_t)main;
	task->regs.cr3=mainTask.regs.cr3;
	task->regs.esp=memman->alloc_4k(64*1024)+64*1024;
	task->next=&mainTask;
	lastTask->next=task;
}

void preempt_exit(){
	auto last=runningTask;
	runningTask=runningTask->next;
	switchTask(&last->regs,&runningTask->regs);
}

void exitTask(){
	prevTask->next=runningTask->next;
	memman->free_4k(runningTask->regs.esp,64*1024);
	memman->free((intptr_t)runningTask,sizeof(Task));
	preempt_exit();
}

void preempt(){
	prevTask=runningTask;
	runningTask=runningTask->next;
	switchTask(&prevTask->regs,&runningTask->regs);
}
