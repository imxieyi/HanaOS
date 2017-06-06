#include "asmfunc.hpp"
#include "timer.hpp"
#include "fifo.hpp"
#include "heap.hpp"
#include "inputdevices.hpp"
#include "task.hpp"

TIMERCTRL *timerctrl;

void init_pit(){
	timerctrl=(TIMERCTRL*)malloc(sizeof(TIMERCTRL));
	register_interrupt_handler(IRQ0,&timer_handler);
	io_out8(PIT_CTRL,0x34);
	io_out8(PIT_CNT0,0x9c);
	io_out8(PIT_CNT0,0x2e);
	timerctrl->count=0;
	for(int i=0;i<MAX_TIMER;i++)
		timerctrl->timer[i].flag=FREE;
	auto t=timer_alloc();
	t->timeout=0xffffffff;
	t->flag=USING;
	t->next=NULL;
	timerctrl->t0=t;
	timerctrl->next=0xffffffff;
	return;
}

registers_t *regists;

void timer_handler(registers_t regs){
	regists=&regs;
	io_out8(PIC0_OCW2,0x60);
	timerctrl->interrupt();
	return;
}

extern TIMER *mt_timer;

void TIMERCTRL::interrupt(){
	count++;
	char ts=0;
	if(next>count)return;
	auto timer=t0;
	for(;;){
		if(timer->timeout>count)break;
		if(timer!=mt_timer){
			timer->push();
		}else{
			timer->flag=ALLOC;
			ts=1;
		}
		timer=timer->next;
	}
	t0=timer;
	next=t0->timeout;
	if(ts!=0)mt_taskswitch();
	return;
}

void TIMERCTRL::regist(TIMER *timer){
	auto t=t0;
	if(timer->timeout<=t->timeout){
		t0=timer;
		timer->next=t;
		next=timer->timeout;
		return;
	}
	TIMER *s;
	for(;;){
		s=t;
		t=t->next;
		if(t==NULL)break;
		if(timer->timeout<=t->timeout){
			s->next=timer;
			timer->next=t;
			return;
		}
	}
	return;
}

TIMER *timer_alloc(){
	return timerctrl->alloc();
}

TIMER *TIMERCTRL::alloc(){
	for(int i=0;i<MAX_TIMER;i++)
		if(timer[i].flag==FREE){
			timer[i].flag=ALLOC;
			return &timer[i];
		}
	return 0;
}

void TIMER::push(){
	flag=ALLOC;
	fifo->put(data);
}

void TIMER::free(){
	flag=FREE;
	return;
}

TIMER *TIMER::init(FIFO *fifo,int data){
	this->fifo=fifo;
	this->data=data;
	return this;
}

void TIMER::setdata(int data){
	this->data=data;
}

void TIMER::set(unsigned int timeout){
	this->timeout=timerctrl->count+timeout;
	flag=USING;
	int e=io_load_eflags();
	io_cli();
	timerctrl->regist(this);
	io_store_eflags(e);
	return;
}
