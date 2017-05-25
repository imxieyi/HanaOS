#ifndef _TASK_HPP
#define _TASK_HPP

#include <stdint.h>

typedef struct {
	uint32_t eax,ebx,ecx,edx,esi,edi,esp,ebp,eip,eflags,cr3;
} __attribute__((packed)) Registers;

typedef struct Task {
	Registers regs;
	struct Task *next;
} __attribute__((packed)) Task;

extern "C" void switchTask(Registers *from, Registers *to);
void createTask(void (*main)());
void exitTask();
void initTasking();
void preempt();

#endif
