#pragma once

#include "fifo.hpp"
#include "isr.hpp"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040
#define MAX_TIMER	500
enum TIMER_FLAG {FREE,ALLOC,USING};
class TIMER {
private:
	int data;
public:
	FIFO *fifo;
	TIMER *next;
	TIMER_FLAG flag;
	unsigned int timeout;
	void set(unsigned int timeout);
	void push();
	void free();
	TIMER *init(FIFO *fifo,int data);
	void setdata(int data);
};
class TIMERCTRL {
public:
	unsigned int count,next;
	TIMER *t0;
	TIMER timer[MAX_TIMER];
	TIMER *alloc();
	void interrupt();
	void regist(TIMER *timer);
};
void init_pit();
TIMER *timer_alloc();
void timer_handler(registers_t regs);
