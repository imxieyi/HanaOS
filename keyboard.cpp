#include "asmfunc.hpp"
#include "fifo.hpp"
#include "inputdevices.hpp"
#include "isr.hpp"

FIFO *keyfifo;
int keydata0;
void wait_kbc_sendready(void){
	//等待键盘控制电路准备完毕
	for(;;)
		if((io_in8(PORT_KEYSTA)&KEYSTA_SEND_NOTREADY)==0)
			return;
}

void init_keyboard(FIFO *fifo,int data0){
	keyfifo=fifo;
	keydata0=data0;
	wait_kbc_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE);
	wait_kbc_sendready();
	io_out8(PORT_KEYDAT,KBC_MODE);
	return;
}

//来自PS/2键盘的中断
void keyboard_interrupt(registers_t regs){
	(void)regs;
	int data;
	io_out8(PIC0_OCW2, 0x61);	//通知PIC IRQ-01已经受理完毕
	data=io_in8(PORT_KEYDAT);
	keyfifo->put(data+keydata0);
	return;
}
