#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "heap.hpp"
#include "apps.hpp"
#include "apps_api.hpp"
using namespace hanastd;

extern MEMMAN *memman;
extern unsigned int memtotal;

void *app_free(char *buffer, uint32_t *cbuffer, char *param) {
	STDOUT out(buffer,cbuffer);
	int total=memtotal/1024;
	int free=memman->total()/1024;
	out.printf(0xceff85,"Memory info:\n");
	out.printf(0xff85bb,"Total: ");
	out.printf(0xffce85,"%dKB\n",total);
	out.printf(0xff85bb,"In use: ");
	out.printf(0xffce85,"%dKB\n",(total-free));
	out.printf(0xff85bb,"Free: ");
	out.printf(0xffce85,"%dKB\n",free);
}
