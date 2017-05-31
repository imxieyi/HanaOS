#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"

void *app_reboot(char *buffer, uint32_t *cbuffer, char *param) {
	reboot();
}
