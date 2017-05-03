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
#include "hanastd.hpp"
using namespace hanastd;

static void key_handler(registers_t regs){
	setcolor(0x0000ff);
	putstr("Keyboard!",2,50,200);
	for(;;)
		io_hlt();
}
uint32_t tick=0;
static void timer_handler(registers_t regs){
	(void)regs;
	if(tick==4294967295)tick=0;
	else tick++;
	setcolor(0x00ff00);
	putstr("Timer!",2,50,250);
}
void timer_init(uint32_t frequency){
	register_interrupt_handler(IRQ0,&timer_handler);
	uint32_t divisor=1193180/frequency;
	io_out8(0x43,0x36);
	uint8_t l=(uint8_t)(divisor&0xff);
	uint8_t h=(uint8_t)((divisor>>8)&0xff);
	io_out8(0x40,l);
	io_out8(0x40,h);
}
#ifdef __cplusplus
extern "C"{
#endif
extern uint32_t kmalloc_addr;
void kernel_main(multiboot_info_t *hdr,uint32_t magic)
{
	init_graphics((vbe_mode_info_t*)hdr->vbe_mode_info);
    show_bgimg();
	setcolor(0x66ccff);
	boxfill(0,100,500,300);
	setcolor(0xff0000);
	//Memory Test
	unsigned int memtotal;
	memtotal=memtest(kmalloc_addr,0xbfffffff);
	auto *memman=(MEMMAN*)kmalloc_a(sizeof(MEMMAN));
	memman->init();
	memman->free(kmalloc_addr,memtotal-kmalloc_addr);
	
	char str[32];
	sprintf(str,"mem %dMB free:%dKB",memtotal/0x400000*4,memman->total()/1024);
	putstr(str,2,50,110);
	setcolor(0x2a6927);
	putstr("Hello HanaOS!",2,50,150);
	gdt_init();
	idt_init();
	register_interrupt_handler(IRQ1,&key_handler);
	timer_init(10);
	paging_init();
	io_sti();
	for(;;)
		io_hlt();
}
#ifdef __cplusplus
}
#endif
