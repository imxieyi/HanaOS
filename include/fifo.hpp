#pragma once

#define FLAGS_OVERRUN	0x0001
class FIFO {
private:
	int r,w,size,free,flags;
public:
	int *buf;
	struct Task *task;
	void init(int size);
	void init(int size,Task *task);
	int put(int data);
	int get();
	void remove();
	int status();
};
