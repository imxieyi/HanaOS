#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "sheet.hpp"
#include "task.hpp"
#include "fifo.hpp"
#include "heap.hpp"
#include "timer.hpp"
#include "rtc.hpp"
#include "inputdevices.hpp"
#include "graphics.hpp"
#include "sheet.hpp"
#include "appstore.hpp"
#include "hanastd.hpp"
using namespace hanastd;

#define CONSOLE_BG 0xaa2c3a44
#define FONT_COLOR 0x74e3ff
#define ERROR_COLOR 0xffafaf
void task_console(void *arg){
	auto sht=(SHEET*)arg;
	sht->graphics->init_window("Console");
	sht->graphics->make_textbox(8,30,632,472,CONSOLE_BG,true);
	sht->slide(200,200);
	sht->updown(2);
	auto task=task_now();
	auto fifo=(FIFO*)malloc(sizeof(FIFO));
	fifo->init(128,task);
	task->fifo=fifo;
	auto timer=timer_alloc()->init(fifo,1);
	timer->set(50);
	uint32_t cursor_c=0x000000;
	int columns=(sht->graphics->width-16)/8;
	int lines=(sht->graphics->height-37)/16;
	int cursor_x=8,cursor_y=30;
	int i;
	char cmdbuffer[128];
	int cmdlen=0;

	auto push_char = [&](char ch,uint32_t color) {
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
			sht->putstring(cursor_x,cursor_y,1,color,CONSOLE_BG,true,s);
			cursor_x+=8;
		}
	};

	auto stdout=(char*)malloc(16*1024);
	auto stdoutcolor=(uint32_t*)malloc(64*1024);

	auto push_char_keyboard = [&](char ch) {
		if(ch=='\n'){
			push_char(ch,FONT_COLOR);
			if(cmdlen>0){
				cmdbuffer[cmdlen]=0;
				auto app=appstore_search(cmdbuffer);
				if(app!=NULL){
					memset(stdout,0,16*1024);
					app->entry(stdout,stdoutcolor,NULL);
					//if(!app->newtask){
						for(int i=0;i<16*1024&&stdout[i]!=0;i++)
							push_char(stdout[i],stdoutcolor[i]);
					//}
				} else {
					sprintf(stdout,"App not found: ");
					for(int i=0;i<16*1024&&stdout[i]!=0;i++)
						push_char(stdout[i],ERROR_COLOR);
					sprintf(stdout,"%s\n",cmdbuffer);
					for(int i=0;i<16*1024&&stdout[i]!=0;i++)
						push_char(stdout[i],0xffd9af);
				}
				push_char('\n',0);
				cmdlen=0;
			}
			sht->putstring(cursor_x,cursor_y,1,0x74ff84,CONSOLE_BG,true,"$");
			cursor_x=24;
		}else{
			if(cmdlen<columns-2){
				cmdbuffer[cmdlen]=ch;
				cmdlen++;
			}
			push_char(ch,FONT_COLOR);
		}
	};

	//Wait for RTC
	extern bool timeset;
	while(!timeset)io_hlt();
	
	//Welcome message
	auto time=rtc_time();
	time2str(&time,cmdbuffer);
	sprintf(stdout,"%s",cmdbuffer);
	sht->putstring(cursor_x,cursor_y,1,0xd3b1ff,CONSOLE_BG,true,stdout);
	cursor_y+=16;
	sprintf(cmdbuffer,"Welcome to HanaOS!");
	sht->putstring(cursor_x,cursor_y,1,0xb1e5ff,CONSOLE_BG,true,cmdbuffer);
	cursor_y+=16;

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
					cursor_c=0xffffffff;
				}else{
					timer->setdata(1);
					cursor_c=CONSOLE_BG;
				}
				timer->set(50);
				sht->graphics->setcolor(cursor_c,true);
				sht->graphics->boxfill(cursor_x,cursor_y,cursor_x+7,cursor_y+15);
				sht->refresh(cursor_x,cursor_y,cursor_x+7,cursor_y+15);
			}else if(i>=256&&i<=511){
				if(i == 0x0e + 256){//退格键
					if(cursor_x>24){
						cmdlen--;
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
