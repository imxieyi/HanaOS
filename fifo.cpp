#include "fifo.hpp"
#include "heap.hpp"
#include "task.hpp"

void FIFO::init(int size){
	this->size=size;
	this->task=0;
	buf=(int *)malloc(size*sizeof(int));
	free=size;
	flags=0;
	w=0;
	r=0;
	return;
}

void FIFO::init(int size,Task *task){
	this->size=size;
	this->task=task;
	buf=(int *)malloc(size*sizeof(int));
	free=size;
	flags=0;
	w=0;
	r=0;
	return;
}

void FIFO::remove(){
	mfree((uintptr_t)buf,size*4);
	return;
}

int FIFO::put(int data){
	if(free==0){
		flags|=FLAGS_OVERRUN;
		return -1;
	}
	buf[w]=data;
	w=(w+1)%size;
	free--;
	if(task!=0)
		if(task->stat==SLEEPING)
			task_run(task,-1,0);
	return 0;
}

int FIFO::get(){
	int data;
	if(free==size){
		return -1;
	}
	data=buf[r];
	r=(r+1)%size;
	free++;
	return data;
}

int FIFO::status(){
	return size-free;
}
