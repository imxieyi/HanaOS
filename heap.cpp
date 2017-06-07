#include <stddef.h>
#include <stdint.h>
#include "heap.hpp"
#include "asmfunc.hpp"
#include "hanastd.hpp"

MEMMAN *memman;
unsigned int memtotal;

/* a simple pointer-arithmetic-based malloc */
#define PAGE_MASK 0xFFFFF000
#define PAGE_SIZE     0x1000
/* a symbol put there by the linker */
extern char link_mem_end;
uint32_t kmalloc_addr = (uintptr_t)&link_mem_end;

static void *kmalloc_int(size_t sz, int align, void **phys)
{
    if(align)
    {
        kmalloc_addr += (PAGE_SIZE - 1);
        kmalloc_addr &= PAGE_MASK;
    }
    if(phys)
        *phys = (void*)kmalloc_addr;
    void *ret = (void*)kmalloc_addr;
    kmalloc_addr += sz;
    /* make sure the address is aligned if requested */
    //assert(!align || (((uint32_t)ret & 0xFFF) == 0));
    return ret;
}

void *kmalloc(size_t sz)
{
    return kmalloc_int(sz, 0, NULL);
}

void *kmalloc_a(size_t sz)
{
    return kmalloc_int(sz, 1, NULL);
}

void *kmalloc_p(size_t sz, void **phys)
{
     return kmalloc_int(sz, 0, phys);
}

void *kmalloc_ap(size_t sz, void **phys)
{
    return kmalloc_int(sz, 1, phys);
}

void kmalloc_set_addr(uint32_t addr)
{
    kmalloc_addr = addr;
}

void memman_init(){
	//Memory Test & MEMMAN init
	memtotal=memtest(kmalloc_addr,0xbfffffff);
	memman=(MEMMAN*)kmalloc_a(sizeof(MEMMAN));
	memman->init();
	mfree(kmalloc_addr,memtotal-kmalloc_addr);
	hanastd::memset((void*)kmalloc_addr,0,memman->total());
}

void MEMMAN::init(){
	this->frees=0;	//可用信息数
	this->maxfrees=0;//frees的最大值
	this->lostsize=0;//释放失败内存大小和
	this->losts=0;	//释放失败次数
	return;
}

unsigned int MEMMAN::total(){
	unsigned int i,t=0;
	for(i=0;i<frees;i++)
		t+=freeinfo[i].size;
	return t;
}

unsigned int MEMMAN::alloc(unsigned int size){
//	return kmalloc_a(size);
	unsigned int i,a;
	for(i=0;i<this->frees;i++)
		if(this->freeinfo[i].size>=size){
			a=this->freeinfo[i].addr;
			this->freeinfo[i].addr+=size;
			this->freeinfo[i].size-=size;
			if(this->freeinfo[i].size==0){
				this->frees--;
				for(;i<this->frees;i++)
					this->freeinfo[i]=this->freeinfo[i+1];
			}
			return a;
		}
	return 0;
}

int MEMMAN::free(unsigned int addr,unsigned int size){
	int i,j;
	for(i=0;i<this->frees;i++)
		if(this->freeinfo[i].addr>addr)
			break;
	if(i>0)
		if(this->freeinfo[i-1].addr+this->freeinfo[i-1].size==addr){
			this->freeinfo[i-1].size+=size;
			if(i<this->frees)
				if(addr+size==this->freeinfo[i].addr){
					this->freeinfo[i-1].size+=this->freeinfo[i].size;
					this->frees--;
					for(;i<this->frees;i++)
						this->freeinfo[i]=this->freeinfo[i+1];
				}
			return 0;
		}
	if(i<this->frees)
		if(addr+size==this->freeinfo[i].addr){
			this->freeinfo[i].addr=addr;
			this->freeinfo[i].size+=size;
			return 0;
		}
	if(this->frees<MEMMAN_FREES){
		for(j=this->frees;j>i;j--)
			this->freeinfo[j]=this->freeinfo[j-1];
		this->frees++;
		if(this->maxfrees<this->frees)
			this->maxfrees=this->frees;
		this->freeinfo[i].addr=addr;
		this->freeinfo[i].size=size;
		return 0;
	}
	this->losts++;
	this->lostsize+=size;
	return -1;
}

unsigned int MEMMAN::alloc_4k(unsigned int size){
	size=(size+0xfff)&0xfffff000;
	return this->alloc(size);
//	return kmalloc_a(size);
}

int MEMMAN::free_4k(unsigned int addr,unsigned int size){
	size=(size+0xfff)&0xfffff000;
	return this->free(addr,size);
}

uintptr_t malloc(uint32_t size){
	auto t=memman->alloc_4k(size);
	hanastd::memset((uintptr_t)t,0,size);
	return t;
}

int mfree(uintptr_t addr,uint32_t size){
	return memman->free_4k(addr,size);
}
