#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "graphics.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "isr.hpp"
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
void kernel_main(multiboot_info_t *hdr,uint32_t magic)
{
	init_graphics((vbe_mode_info_t*)hdr->vbe_mode_info);
    show_bgimg();
	setcolor(0x66ccff);
	boxfill(0,100,500,300);
	setcolor(0xff0000);
	putstr("ABC 123",2,50,110);
	setcolor(0x2a6927);
	putstr("Hello HanaOS!",2,50,150);
	gdt_init();
	idt_init();
	register_interrupt_handler(IRQ1,&key_handler);
	io_sti();
	timer_init(10);
	for(;;)
		io_hlt();
}
#ifdef __cplusplus
}
#endif
