#ifndef _FIFO_HPP
#define _FIFO_HPP

#define FLAGS_OVERRUN	0x0001
class FIFO {
private:
	class MEMMAN *memman;
	int r,w,size,free,flags;
public:
	int *buf;
	struct Task *task;
	void init(class MEMMAN *memman,int size);
	void init(class MEMMAN *memman,int size,Task *task);
	int put(int data);
	int get();
	void remove();
	int status();
};

#endif
