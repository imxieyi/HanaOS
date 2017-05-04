#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "graphics.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "isr.hpp"
#include "paging.hpp"
#include "heap.hpp"
#include "sheet.hpp"
#include "inputdevices.hpp"
#include "fifo.hpp"
#include "timer.hpp"
#include "hanastd.hpp"
using namespace hanastd;

SHEET *sht_back,*sht_win;
uint32_t tick=0;
char buff[32];
/*static void timer_handler(registers_t regs){
	(void)regs;
	if(tick==4294967295)tick=0;
	else tick++;
	sprintf(buff,"Tick: %d",tick);
	sht_win->putstring(10,50,2,0x000000,sht_win->graphics->bgcolor,buff);
}*/
void timer_init(uint32_t frequency){
	register_interrupt_handler(IRQ0,&timer_handler);
	uint32_t divisor=1193180/frequency;
	io_out8(0x43,0x36);
	uint8_t l=(uint8_t)(divisor&0xff);
	uint8_t h=(uint8_t)((divisor>>8)&0xff);
	io_out8(0x40,l);
	io_out8(0x40,h);
}
extern TIMERCTRL *timerctrl;
#ifdef __cplusplus
extern "C"{
#endif
extern uint32_t kmalloc_addr;
void kernel_main(multiboot_info_t *hdr,uint32_t magic)
{
	//Memory Test & MEMMAN init
	unsigned int memtotal;
	memtotal=memtest(kmalloc_addr,0xbfffffff);
	auto *memman=(MEMMAN*)kmalloc_a(sizeof(MEMMAN));
	memman->init();
	memman->free(kmalloc_addr,memtotal-kmalloc_addr);
	memset((void*)kmalloc_addr,0,memman->total());

	//Init Sheetctrl
	auto shtctl=sheetctrl_init(memman,(vbe_mode_info_t*)hdr->vbe_mode_info);

	//Init background
	sht_back=shtctl->allocsheet(shtctl->xsize,shtctl->ysize);
	sht_back->graphics->show_bgimg();
	sht_back->slide(0,0);
	sht_back->updown(0);
	sht_back->graphics->setcolor(0x66ccff);
	sht_back->graphics->boxfill(0,100,500,600);
	shtctl->refreshall(0,0,1024,768);

	char str[128];
	sht_back->putstring(50,150,2,0x2a6927,0x66ccff,"Hello HanaOS!");
	
	auto test_win=shtctl->allocsheet(320,100);
	test_win->graphics->init_window("mouse");
	test_win->slide(250,250);
	test_win->updown(1);
//	test_win->putstring(10,50,2,0xff0000,test_win->graphics->bgcolor,"I am a window");

	//Init window
	sht_win=shtctl->allocsheet(320,100);
	sht_win->graphics->init_window("timer");
	sht_win->slide(320,300);
	sht_win->updown(2);
	
	//Init timer
	timerctrl=(TIMERCTRL*)memman->alloc_4k(sizeof(TIMERCTRL));

	gdt_init();
	idt_init();
	init_pit();
//	paging_init();
	
	//Keyboard init
	auto fifo=(FIFO*)memman->alloc_4k(sizeof(FIFO));
	fifo->init(memman,128);
	init_keyboard(fifo,256);
	register_interrupt_handler(IRQ1,&keyboard_interrupt);
	
	//Mouse init
	struct MOUSE_DEC mdec;
	enable_mouse(fifo,512,&mdec);
	register_interrupt_handler(IRQ12,&mouse_handler);
	
	//Mouse cursor init
	int mx=shtctl->xsize/2;
	int my=shtctl->ysize/2;
	auto mouse_sht=shtctl->allocsheet(20,30);
	mouse_sht->graphics->init_mouse_cursor();
	mouse_sht->slide(mx,my);
	mouse_sht->updown(3);
	
	auto timer1=timerctrl->alloc()->init(fifo,10);
	auto timer2=timerctrl->alloc()->init(fifo,3);
	auto timer3=timerctrl->alloc()->init(fifo,1);
	timer1->set(1000);
	timer2->set(300);
	timer3->set(50);
	io_sti();
	sprintf(str,"mem %dMB free:%dKB",memtotal/0x400000*4,memman->total()/1024);
	sht_back->putstring(50,110,2,0xff0000,0x66ccff,str);
	sprintf(str,"sht_back: %d",sht_back->graphics->vram);
	sht_back->putstring(50,400,2,0xff0000,0x66ccff,str);
	sprintf(str,"test_win: %d",test_win->graphics->vram);
	sht_back->putstring(50,450,2,0xff0000,0x66ccff,str);
	sprintf(str,"sht_win: %d",sht_win->graphics->vram);
	sht_back->putstring(50,500,2,0xff0000,0x66ccff,str);
	sprintf(str,"mouse_sht: %d",mouse_sht->graphics->vram);
	sht_back->putstring(50,550,2,0xff0000,0x66ccff,str);

	int i=0;
	for(;;){
		io_cli();
		sprintf(str,"%d",timerctrl->count);
		sht_win->putstring(10,50,2,0x000000,sht_win->graphics->bgcolor,str);
		if(fifo->status()==0){
			io_stihlt();
		}else{
			i=fifo->get();
			io_sti();
			if(i>=256&&i<=511){
				sprintf(str,"%02X",i-256);
				sht_back->putstring(50,200,2,0x0000ff,0x66ccff,str);
			}else if(i>=512&i<=767){
				if(mouse_decode(&mdec,i-512)|1){
					sprintf(str,"lcr %4d %4d",mdec.x,mdec.y);
					if(mdec.btn&0x01)str[0]='L';
					if(mdec.btn&0x02)str[2]='R';
					if(mdec.btn&0x04)str[1]='C';
					test_win->putstring(10,50,2,0xff0000,test_win->graphics->bgcolor,str);
//					sht_back->putstring(90,200,2,0x0000ff,0x66ccff,str);
					mx+=mdec.x;
					my+=mdec.y;
					mx=mx<0?0:mx;
					my=my<0?0:my;
					mx=mx>shtctl->xsize-1?shtctl->xsize-1:mx;
					my=my>shtctl->ysize-1?shtctl->ysize-1:my;
					mouse_sht->slide(mx,my);
					sprintf(str,"(%4d,%4d)",mx,my);
//					sht_back->putstring(300,150,2,0x2a6927,0x66ccff,str);
				}
			}else if(i==10){
				sht_back->putstring(50,250,2,0x000000,0x66ccff,"10[sec]");
			}else if(i==3){
				sht_back->putstring(50,250,2,0x000000,0x66ccff,"3[sec]");
			}else if(i==1){
				timer3->setdata(0);
				sht_back->graphics->setcolor(0,0,0);
				sht_back->graphics->boxfill(170,250,180,280);
				timer3->set(50);
				shtctl->refreshall(170,250,180,280);
			}else if(i==0){
				timer3->setdata(1);
				sht_back->graphics->setcolor(0x66ccff);
				sht_back->graphics->boxfill(170,250,180,280);
				timer3->set(50);
				shtctl->refreshall(170,250,180,280);
			}
		}
	}
}
#ifdef __cplusplus
}
#endif
