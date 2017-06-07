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
#include "rtc.hpp"
#include "fifo.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "dwm.hpp"
#include "appstore.hpp"
#include "hanastd.hpp"
using namespace hanastd;

SHEET *sht_back,*mouse_sht;
extern void task_console(void *arg);
#define CONSOLE_BG 0xaa000000

extern "C" void kernel_main(multiboot_info_t *hdr,uint32_t magic)
{
	memman_init();
	
	//Init Sheetctrl
	auto vbeinfo=(vbe_mode_info_t*)hdr->vbe_mode_info;
	shtctl_init(vbeinfo);
	
	int xsize=vbeinfo->Xres,ysize=vbeinfo->Yres;
	
	auto sht_bg=shtctl_alloc(xsize,ysize);
	sht_bg->graphics->show_bgimg();
	sht_bg->slide(0,0);
	sht_bg->updown(0);

	//Init background
	sht_back=shtctl_alloc(xsize,ysize);
	sht_back->slide(0,0);
	sht_back->updown(1);
	shtctl_refresh(0,0,1024,768);

	gdt_init();
	idt_init();
	init_pit();
	
	//RTC
	init_rtc();
	
	//Mouse cursor init
	int mx=xsize/2,my=ysize/2;
	mouse_sht=shtctl_alloc(20,30);
	mouse_sht->graphics->init_mouse_cursor();
	mouse_sht->slide(mx,my);
	mouse_sht->updown(3);
	
	//Console Window
	auto con_sht=shtctl_alloc(800,600);
	dwm_init(con_sht);

	//Multitasking
	auto mt_timer=timer_alloc();
	auto taska=initTasking(mt_timer);
	auto task_con=createTask("console",&task_console,con_sht);
	task_run(task_con,1,1);
	
	//Keyboard init
	auto fifo=(FIFO*)malloc(sizeof(FIFO));
	fifo->init(128,taska);
	init_keyboard(fifo,256);
	register_interrupt_handler(IRQ1,&keyboard_interrupt);
	
	//Mouse init
	struct MOUSE_DEC mdec;
	enable_mouse(fifo,512,&mdec);
	register_interrupt_handler(IRQ12,&mouse_handler);
	
	//AppStore
	appstore_init();

	io_sti();

	bool mousepressed=false;
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
					mx=mx>xsize-1?xsize-1:mx;
					my=my>ysize-1?ysize-1:my;
					mouse_sht->slide(mx,my);
					if(mdec.btn&0x01){
						if(mousepressed){
							dwm_mousedragged(mdec.x,mdec.y);
						}else{
							dwm_mousepressed(mx,my);
							mousepressed=true;
						}
					}else{
						if(mousepressed){
							dwm_mousereleased();
							mousepressed=false;
						}
					}
				}
			}else if(i>=256&&i<=511){
				if(task_con->fifo!=NULL)
					task_con->fifo->put(i);
			}
		}
	}
}
