#pragma once

#include <stddef.h>
#include <stdint.h>
void *kmalloc(size_t);
void *kmalloc_a(size_t);
void *kmalloc_p(size_t, void**);
void *kmalloc_ap(size_t, void**);
void kmalloc_set_addr(uint32_t);

//OSASK Memory Manager
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000
#define MEMMAN_FREES		4000
//#define MEMMAN_ADDR			0x003c0000
struct FREEINFO{
	unsigned int addr,size;
};
class MEMMAN {
private:
	int frees,maxfrees,lostsize,losts;
public:
	struct FREEINFO freeinfo[MEMMAN_FREES];
	void init();
	unsigned int total();
	unsigned int alloc(unsigned int size);
	int free(unsigned int addr,unsigned int size);
	unsigned int alloc_4k(unsigned int size);
	int free_4k(unsigned int addr,unsigned int size);
};
extern "C" int load_cr0(void);
extern "C" void store_cr0(int cr0);
extern "C" unsigned int memtest(unsigned int start,unsigned int end);
void memman_init();
uintptr_t malloc(uint32_t size);
int mfree(uintptr_t addr,uint32_t size);
