#include "hanastd.hpp"
#include "apps_api.hpp"
using namespace hanastd;

STDOUT::STDOUT(char *buffer, uint32_t *cbuffer){
	this->buffer=buffer;
	this->cbuffer=cbuffer;
	this->offset=0;
}

int STDOUT::printf(uint32_t color,const char *format, ...){
	char buf[128];
	int i;
	va_list ap;

	va_start(ap, format);
	i = vsprintf(buf, format, ap);
	va_end(ap);
	append(buf,color);
	return i;
}

void STDOUT::append(char* str, uint32_t color){
	int len=strlen(str);
	for(int i=0;i<len;i++){
		buffer[offset+i]=str[i];
		cbuffer[offset+i]=color;
	}
	offset+=len-1;
	buffer[offset]=0;
}

