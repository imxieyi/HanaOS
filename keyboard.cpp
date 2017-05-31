#include "asmfunc.hpp"
#include "fifo.hpp"
#include "inputdevices.hpp"
#include "isr.hpp"

static char keytable_noshift[0x7f]={
	0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,'q','w','e','r','t','y','u','i','o','p','[',']',0,0,'a','s',
	'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static char keytable_withshift[0x7f]={
	0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,'A','S',
	'D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','？',0,'*',0,' ',0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

bool lshift=false,rshift=false;

char getchar(uint8_t code){
	if(code==0x2a)lshift=true;
	if(code==0x36)rshift=true;
	if(code==0xaa)lshift=false;
	if(code==0xb6)rshift=false;
	if(code<=0x7f){
		if(lshift||rshift)
			return keytable_withshift[code];
		else return keytable_noshift[code];
	}
	return 0;
}

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
