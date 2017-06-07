#include "hanastd.hpp"
#include "apps_api.hpp"
#include "graphics.hpp"
#include "sheet.hpp"
#include "heap.hpp"
#include "task.hpp"
#include "dwm.hpp"
using namespace hanastd;

STDOUT::STDOUT(char *buffer, uint32_t *cbuffer){
	this->buffer=buffer;
	this->cbuffer=cbuffer;
	this->offset=0;
}

int STDOUT::printf(uint32_t color,const char *format, ...){
	char buf[128];
	int i;
	va_list ap;

	va_start(ap, format);
	i = vsprintf(buf, format, ap);
	va_end(ap);
	append(buf,color);
	return i;
}

void STDOUT::append(char* str, uint32_t color){
	int len=strlen(str);
	for(int i=0;i<len;i++){
		buffer[offset+i]=str[i];
		cbuffer[offset+i]=color;
	}
	offset+=len-1;
	buffer[offset]=0;
}

SHEET *init_window(int width, int height, char *title){
	auto sht=shtctl_alloc(width,height);
	sht->graphics->init_window(title);
	sht->slide((screen_width-width)/2,(screen_height-height)/2);
	dwm_addtop(sht,task_now());
	return sht;
}

void close_window(SHEET *sht){
	dwm_removewindow(sht);
}

void api_loopforever(LambdaContainer func,int interval){
	auto task=task_now();
	task->callback=&func;
	auto fifo=(FIFO*)malloc(sizeof(FIFO));
	fifo->init(128,task);
	task->fifo=fifo;
	auto timer=timer_alloc()->init(fifo,1);
	timer->set(interval);
	for(;;){
		if(!fifo->status()){
			sleepTask();
		}else{
			fifo->get();
			timer->set(interval);
			func();
		}
	}
}
