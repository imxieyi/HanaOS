#ifndef _TASK_HPP
#define _TASK_HPP

#include <stdint.h>
#include "timer.hpp"

typedef struct {
	uint32_t eax,ebx,ecx,edx,esi,edi,esp,ebp,eip,eflags,cr3;
} __attribute__((packed)) Registers;

enum TASKSTATUS{EMPTY,RUNNING,SLEEPING};
typedef struct Task {
	Registers regs;
	TASKSTATUS stat=RUNNING;
} __attribute__((packed)) Task;

#define MAX_TASKS 1000

class TASKCTRL {
public:
	int running;
	int now;
	Task *tasks[MAX_TASKS];
	Task tasks0[MAX_TASKS];
	Task *alloc();
};

extern "C" void switchTask(Registers *from, Registers *to);
extern "C" void mt_taskswitch();
Task *createTask(void (*main)());
void task_run(Task *task);
void exitTask();
void sleepTask();
Task *initTasking(TIMER *timer);
void preempt();

#endif
