#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "sheet.hpp"
#include "task.hpp"
#include "fifo.hpp"
#include "heap.hpp"
#include "timer.hpp"
#include "inputdevices.hpp"
#include "graphics.hpp"
#include "sheet.hpp"

extern MEMMAN *memman;
extern TIMERCTRL *timerctrl;

#define CONSOLE_BG 0xaa000000
#define FONT_COLOR 0x74e3ff
void task_console(void *arg){
	auto sht=(SHEET*)arg;
	auto task=task_now();
	auto fifo=(FIFO*)memman->alloc(sizeof(FIFO));
	fifo->init(memman,128,task);
	task->fifo=fifo;
	auto timer=timerctrl->alloc()->init(fifo,1);
	timer->set(50);
	uint32_t cursor_c=0x000000;
	int cursor_x=8,cursor_y=30;
	int i;
	char s[32];

	auto push_char = [&](char ch) {
		if(ch=='\n'){
			sht->putstring(cursor_x,cursor_y,1,0,CONSOLE_BG,true," ");
			cursor_y+=16;
			cursor_x=8;
			sht->putstring(cursor_x,cursor_y,1,0x74ff84,CONSOLE_BG,true,"$");
			cursor_x=24;
		}else if(cursor_x<sht->graphics->width-22){
			char s[2];
			s[0]=ch;
			s[1]=0;
			sht->putstring(cursor_x,cursor_y,1,FONT_COLOR,CONSOLE_BG,true,s);
			cursor_x+=8;
		}
	};

	sht->putstring(cursor_x,cursor_y,1,0x74ff84,CONSOLE_BG,true,"$");
	cursor_x+=16;
	for(;;){
		io_cli();
		if(!fifo->status()){
			sleepTask();
			io_sti();
		}else{
			i=fifo->get();
			io_sti();
			if(i<=1){
				if(i!=0){
					timer->setdata(0);
					cursor_c=0xffffff;
				}else{
					timer->setdata(1);
					cursor_c=0x000000;
				}
				timer->set(50);
				sht->graphics->setcolor(cursor_c);
				sht->graphics->boxfill(cursor_x,cursor_y,cursor_x+7,cursor_y+15);
				sht->refresh(cursor_x,cursor_y,cursor_x+7,cursor_y+15);
			}else if(i>=256&&i<=511){
				if(i == 0x0e + 256){//退格键
					if(cursor_x>24){
						cursor_x-=8;
						sht->putstring(cursor_x,cursor_y,1,0,CONSOLE_BG,true," ");
					}
				}else if(i == 0x1c + 256){//回车键
					push_char('\n');
				}else{
					if(getchar(i-256)!=0)
						push_char(getchar(i-256));
				}
			}
		}
	}
}
