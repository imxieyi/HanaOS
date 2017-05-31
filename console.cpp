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
#include "appstore.hpp"
#include "hanastd.hpp"
using namespace hanastd;

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
	int columns=(sht->graphics->width-16)/8;
	int lines=(sht->graphics->height-37)/16;
	int cursor_x=8,cursor_y=30;
	int i;
	char cmdbuffer[128];
	int cmdlen=0;

	auto push_char = [&](char ch) {
		if(ch=='\n'){
			sht->putstring(cursor_x,cursor_y,1,0,CONSOLE_BG,true," ");
			if(cursor_y>sht->graphics->height-35){
				for(int y=30;y<30+16*(lines-1);y++)
					for(int x=8;x<sht->graphics->width-8;x++)
						sht->graphics->vram[y*sht->graphics->width+x]=sht->graphics->vram[(y+16)*sht->graphics->width+x];
				for(int y=30+16*(lines-1);y<30+16*lines;y++)
					for(int x=8;x<sht->graphics->width-8;x++)
						sht->graphics->vram[y*sht->graphics->width+x]=CONSOLE_BG;
				sht->refresh(8,30,sht->graphics->width-8,30+16*lines);
			}else{
				cursor_y+=16;
			}
			cursor_x=8;
		}else if(cursor_x<sht->graphics->width-22){
			char s[2];
			s[0]=ch;
			s[1]=0;
			sht->putstring(cursor_x,cursor_y,1,FONT_COLOR,CONSOLE_BG,true,s);
			cursor_x+=8;
		}
	};

	auto stdout=(char*)memman->alloc_4k(64*1024);

	auto push_char_keyboard = [&](char ch) {
		if(ch=='\n'){
			push_char(ch);
			if(cmdlen>0){
				cmdbuffer[cmdlen]=0;
				app_func_t app_entry=appstore_search(cmdbuffer);
				if(app_entry!=0){
					app_entry(stdout,NULL);
				} else {
					sprintf(stdout,"App not found: %s",cmdbuffer);
				}
				for(int i=0;i<64*1024&&stdout[i]!=0;i++)
					push_char(stdout[i]);
				push_char('\n');
				cmdlen=0;
			}
			sht->putstring(cursor_x,cursor_y,1,0x74ff84,CONSOLE_BG,true,"$");
			cursor_x=24;
		}else{
			if(cmdlen<columns-2){
				cmdbuffer[cmdlen]=ch;
				cmdlen++;
			}
			push_char(ch);
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
					push_char_keyboard('\n');
				}else{
					if(getchar(i-256)!=0){
						push_char_keyboard(getchar(i-256));
					}
				}
			}
		}
	}
}
