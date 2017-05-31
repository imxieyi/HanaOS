#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

namespace hanastd {
	template <typename T>
	auto strlen(T str){
		auto i=str;
		for(;*i!=0;i++);
		return i-str+1;
	}
	int sprintf(char *s, const char *format, ...);
	void *memset(void *s, int c, uint32_t n);
	void strncpy(const char *src, char *dest, int n);
	bool strncmp(const char *src, const char *dest, int n);
	int memcmp(const void* aptr, const void* bptr, size_t size);
}

int vsprintf(char *s, const char *format, va_list arg);
