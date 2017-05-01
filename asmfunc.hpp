#ifndef _ASMFUNC_HPP
#define _ASMFUNC_HPP

#ifdef __cplusplus
extern "C"{
#endif

void io_hlt();
void io_cli();
void io_sti();
void io_stihlt();
int io_in8(int port);
int io_in16(int port);
int io_in32(int port);
void io_out8(int port,int data);
void io_out16(int port,int data);
void io_out32(int port,int data);
int io_load_eflags();
void io_store_eflags(int eflags);

#ifdef __cplusplus
}
#endif
#endif
