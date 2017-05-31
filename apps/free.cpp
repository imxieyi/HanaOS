#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "heap.hpp"
#include "apps.hpp"
using namespace hanastd;

extern MEMMAN *memman;
extern unsigned int memtotal;

void *app_free(char *buffer, char *param) {
	int total=memtotal/1024;
	int free=memman->total()/1024;
	sprintf(buffer,"Total memory: %dKB\nIn use: %dKB\nFree: %dKB\n",total,(total-free),free);
}
