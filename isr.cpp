#include <stdint.h>
#include "isr.hpp"
#include "graphics.hpp"
#include "sheet.hpp"
#include "timer.hpp"
#include "heap.hpp"
#include "asmfunc.hpp"
#include "hanastd.hpp"
using namespace hanastd;

#define FOREGROUND 0xf2ff91
#define FOREGROUND_DUMP 0xdaffdb
#define BACKGROUND 0xcc000f4b

#define AUTOREBOOT_COUNTDOWN 15

//http://wiki.osdev.org/Exceptions
const char exceptions[32][32]={
	"Divide by Zero",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown",
	"x87 Floating-point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-point Exception",
	"Virtualization Exception",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Security Exception",
	"Unknown",
};

void isr_handler(registers_t regs){
	auto sht_panic=shtctl_alloc(screen_width,screen_height);
	sht_panic->slide(0,0);
	extern SHEETCTRL *shtctl;
	sht_panic->updown(shtctl->top+1);
	sht_panic->graphics->setcolor(BACKGROUND,true);
	sht_panic->graphics->boxfill(0,0,screen_width,screen_height);
	shtctl_refresh(0,0,screen_width,screen_height);
	char str[64];
	int y=0;
	sprintf(str,"Kernel Panic!");
	sht_panic->putstring(0,y,1,FOREGROUND,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"Interrupt Number: %d",regs.int_no);
	sht_panic->putstring(0,y,1,FOREGROUND,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"Description: %s",exceptions[regs.int_no]);
	sht_panic->putstring(0,y,1,FOREGROUND,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"Error Code: %d",regs.err_code);
	sht_panic->putstring(0,y,1,FOREGROUND,BACKGROUND,true,str);
	y+=32;
	sprintf(str,"Registers Dump:");
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EIP:    0x%08X",regs.eip);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"CS:     0x%08X",regs.cs);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EFLAGS: 0x%08X",regs.eflags);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EAX:    0x%08X",regs.eax);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"ECX:    0x%08X",regs.ecx);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EDX:    0x%08X",regs.edx);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EBX:    0x%08X",regs.ebx);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"ESP:    0x%08X",regs.esp);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EBP:    0x%08X",regs.ebp);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"ESI:    0x%08X",regs.esi);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"EDI:    0x%08X",regs.edi);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	y+=16;
	sprintf(str,"DS:     0x%08X",regs.ds);
	sht_panic->putstring(0,y,1,FOREGROUND_DUMP,BACKGROUND,true,str);
	auto fifo=(FIFO*)malloc(sizeof(FIFO));
	fifo->init(128,NULL);
	auto timer=timer_alloc()->init(fifo,1);
	timer->set(100);
	int countdown=AUTOREBOOT_COUNTDOWN;
	y+=32;
	sprintf(str,"Reboot in %d seconds...",countdown);
	sht_panic->putstring(0,y,2,0xfff8e2,BACKGROUND,true,str);
	io_sti();
	for(;;){
		io_cli();
		if(fifo->status()==0){
			io_stihlt();
		}else{
			io_sti();
			fifo->get();
			countdown--;
			sprintf(str,"Reboot in %d seconds...",countdown);
			sht_panic->putstring(0,y,2,0xfff8e2,BACKGROUND,true,str);
			if(countdown<=0){
				reboot();
			}else{
				timer->set(100);
			}
		}
	}
}
