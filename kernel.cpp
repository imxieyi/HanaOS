#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "graphics.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "isr.hpp"
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
extern void task_console(void *arg);
#define CONSOLE_BG 0xaa000000

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
	sht_back->slide(0,0);
	sht_back->updown(1);
	shtctl->refreshall(0,0,1024,768);
	
	//Init timer
	timerctrl=(TIMERCTRL*)memman->alloc_4k(sizeof(TIMERCTRL));

	gdt_init();
	idt_init();
	init_pit();
	
	//Mouse cursor init
	int mx=shtctl->xsize/2;
	int my=shtctl->ysize/2;
	auto mouse_sht=shtctl->allocsheet(20,30);
	mouse_sht->graphics->init_mouse_cursor();
	mouse_sht->slide(mx,my);
	mouse_sht->updown(5);
	
	//Console Window
	auto con_sht=shtctl->allocsheet(640,480);
	con_sht->graphics->init_window("Console");
	con_sht->graphics->make_textbox(8,30,632,472,CONSOLE_BG,true);
	con_sht->slide(50,50);
	con_sht->updown(2);

	//Multitasking
	auto mt_timer=timerctrl->alloc();
	auto taska=initTasking(mt_timer);
	auto task_con=createTask(&task_console,con_sht);
	task_run(task_con,1,1);
	
	//Keyboard init
	auto fifo=(FIFO*)memman->alloc_4k(sizeof(FIFO));
	fifo->init(memman,128,taska);
	init_keyboard(fifo,256);
	register_interrupt_handler(IRQ1,&keyboard_interrupt);
	
	//Mouse init
	struct MOUSE_DEC mdec;
	enable_mouse(fifo,512,&mdec);
	register_interrupt_handler(IRQ12,&mouse_handler);
	
	auto timer3=timerctrl->alloc()->init(fifo,1);
	timer3->set(50);
	io_sti();

	int i=0;
	for(;;){
		io_cli();
		if(fifo->status()==0){
			io_sti();
			sleepTask();
		}else{
			i=fifo->get();
			io_sti();
			if(i>=512&i<=767){
				if(mouse_decode(&mdec,i-512)|1){
					mx+=mdec.x;
					my+=mdec.y;
					mx=mx<0?0:mx;
					my=my<0?0:my;
					mx=mx>shtctl->xsize-1?shtctl->xsize-1:mx;
					my=my>shtctl->ysize-1?shtctl->ysize-1:my;
					mouse_sht->slide(mx,my);
				}
			}else if(i>=256&&i<=511){
				if(task_con->fifo!=NULL)
					task_con->fifo->put(i);
			}
		}
	}
}
