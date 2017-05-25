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
#include "task.hpp"
#include "hanastd.hpp"
using namespace hanastd;

MEMMAN *memman;
SHEET *sht_back,*sht_win;
extern TIMERCTRL *timerctrl;
extern uint32_t kmalloc_addr;

extern "C" void task_b_main(){
	sht_back->putstring(10,500,1,0x000000,0xffffff,"task switch");
	auto fifo=(FIFO*)memman->alloc_4k(sizeof(FIFO));
	fifo->init(memman,128);
	auto timer=timerctrl->alloc()->init(fifo,1);
	timer->set(500);
	int i;
	for(;;){
		io_cli();
		if(fifo->status()==0){
			io_stihlt();
		}else{
			i=fifo->get();
			io_sti();
			if(i==1){
				exitTask();
			}
		}
	}
}

extern "C" void kernel_main(multiboot_info_t *hdr,uint32_t magic)
{
	//Memory Test & MEMMAN init
	unsigned int memtotal;
	memtotal=memtest(kmalloc_addr,0xbfffffff);
	memman=(MEMMAN*)kmalloc_a(sizeof(MEMMAN));
	memman->init();
	memman->free(kmalloc_addr,memtotal-kmalloc_addr);
	memset((void*)kmalloc_addr,0,memman->total());

	//Init Sheetctrl
	auto shtctl=sheetctrl_init(memman,(vbe_mode_info_t*)hdr->vbe_mode_info);
	
	auto sht_bg=shtctl->allocsheet(shtctl->xsize,shtctl->ysize);
	sht_bg->graphics->show_bgimg();
	sht_bg->slide(0,0);
	sht_bg->updown(0);

	//Init background
	sht_back=shtctl->allocsheet(shtctl->xsize,shtctl->ysize);
// 	sht_back->graphics->show_bgimg();
	sht_back->slide(0,0);
	sht_back->updown(1);
	sht_back->graphics->setcolor(0x00000000,true);
	sht_back->graphics->boxfill(0,0,shtctl->xsize,shtctl->ysize);
	sht_back->graphics->setcolor(0xcc66ccff,true);
	sht_back->graphics->boxfill(0,100,500,400);
	shtctl->refreshall(0,0,1024,768);

	char str[128];
	sht_back->putstring(50,150,2,0x2a6927,0xcc66ccff,true,"Hello HanaOS!");
	
	auto test_win=shtctl->allocsheet(320,100);
	test_win->graphics->init_window("mouse");
	test_win->slide(250,250);
	test_win->updown(2);
//	test_win->putstring(10,50,2,0xff0000,test_win->graphics->bgcolor,"I am a window");

	//Init window
	sht_win=shtctl->allocsheet(320,100);
	sht_win->graphics->init_window("timer");
	sht_win->slide(320,300);
	sht_win->updown(3);
	
	//Keyboard window
	auto key_win=shtctl->allocsheet(320,100);
	key_win->graphics->init_window("keyboard");
	key_win->graphics->make_textbox(15,50,250,66,0xffffff);
	key_win->slide(250,360);
	key_win->updown(4);
	
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
	mouse_sht->updown(5);
	
	uint32_t task_b_esp;
	task_b_esp=memman->alloc_4k(64*1024)+64*1024;
	
	//Multitasking
	initTasking();
	createTask(&task_b_main);
	
	auto timer1=timerctrl->alloc()->init(fifo,10);
	auto timer2=timerctrl->alloc()->init(fifo,3);
	auto timer3=timerctrl->alloc()->init(fifo,1);
	timer1->set(1000);
	timer2->set(300);
	timer3->set(50);
	io_sti();

	int cursor_x=16;

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
				sht_back->putstring(50,200,2,0x0000ff,0xcc66ccff,true,str);
				if(256<=i&&i<=511)
					if(getchar(i-256)!=0&&cursor_x<240){
						str[0]=getchar(i-256);
						str[1]=0;
						key_win->putstring(cursor_x,50,1,0x000000,0xffffff,str);
						cursor_x+=8;
					}
					if(i==256+0x0e&&cursor_x>16){
						cursor_x-=8;
						key_win->putstring(cursor_x,50,1,0x000000,0xffffff," ");
					}
					key_win->graphics->boxfill(cursor_x,50,cursor_x+8,65);
			}else if(i>=512&i<=767){
				if(mouse_decode(&mdec,i-512)|1){
					sprintf(str,"lcr %4d %4d",mdec.x,mdec.y);
					if(mdec.btn&0x01)str[0]='L';
					if(mdec.btn&0x02)str[2]='R';
					if(mdec.btn&0x04)str[1]='C';
					test_win->putstring(10,50,2,0xff0000,test_win->graphics->bgcolor,str);
					mx+=mdec.x;
					my+=mdec.y;
					mx=mx<0?0:mx;
					my=my<0?0:my;
					mx=mx>shtctl->xsize-1?shtctl->xsize-1:mx;
					my=my>shtctl->ysize-1?shtctl->ysize-1:my;
					mouse_sht->slide(mx,my);
					sprintf(str,"(%4d,%4d)",mx,my);
					sht_back->putstring(300,150,2,0x2a6927,0xcc66ccff,true,str);
					if((mdec.btn&0x01)!=0)
						key_win->slide(mx-118,my-8);
				}
			}else if(i==10){
				sht_back->putstring(50,250,2,0x000000,0xcc66ccff,true,"10[sec]");
				preempt();
			}else if(i==3){
				sht_back->putstring(50,250,2,0x000000,0xcc66ccff,true,"3[sec]");
				preempt();
			}else if(i==1){
				timer3->setdata(0);
				key_win->graphics->setcolor(0xffffff);
				key_win->graphics->boxfill(cursor_x,50,cursor_x+8,65);
				key_win->refresh(cursor_x,50,cursor_x+7,66);
				timer3->set(50);
				shtctl->refreshall(170,250,180,280);
			}else if(i==0){
				timer3->setdata(1);
				key_win->graphics->setcolor(0x000000);
				key_win->graphics->boxfill(cursor_x,50,cursor_x+8,65);
				key_win->refresh(cursor_x,50,cursor_x+7,66);
				timer3->set(50);
			}
		}
	}
}
