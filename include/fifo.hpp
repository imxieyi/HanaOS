#ifndef _FIFO_HPP
#define _FIFO_HPP

#define FLAGS_OVERRUN	0x0001
class FIFO {
private:
	class MEMMAN *memman;
	int r,w,size,free,flags;
public:
	int *buf;
	void init(class MEMMAN *memman,int size);
	int put(int data);
	int get();
	void remove();
	int status();
};

#endif
