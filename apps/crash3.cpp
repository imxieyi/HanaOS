#include <stdint.h>
#include "apps.hpp"

void *app_crash3(char *buffer, uint32_t *cbuffer, char *param) {
	asm volatile("jmp 0xffffffff");
}
