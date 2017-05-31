#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "apps_api.hpp"
using namespace hanastd;

void *app_hello(char *buffer, uint32_t *cbuffer, char *param) {
	STDOUT out(buffer,cbuffer);
	out.printf(0xfaff85,"Hello");
	out.printf(0xeb85ff," HanaOS!\n");
}
