#ifndef _TASK_HPP
#define _TASK_HPP

#include <stdint.h>
#include "timer.hpp"

typedef struct {
	uint32_t eax,ebx,ecx,edx,esi,edi,esp,ebp,eip,eflags,cr3;
} __attribute__((packed)) Registers;

typedef struct Task {
	Registers regs;
	struct Task *next;
} __attribute__((packed)) Task;

extern "C" void switchTask(Registers *from, Registers *to);
extern "C" void mt_taskswitch();
void createTask(void (*main)());
void exitTask();
void initTasking(TIMER *timer);
void preempt();

#endif
