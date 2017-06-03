#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "apps.hpp"
#include "apps_api.hpp"
using namespace hanastd;

void *app_crash2(char *buffer, uint32_t *cbuffer, char *param) {
	STDOUT out(buffer,cbuffer);
	int a=1,b=0;
	int c=a/b;
	out.printf(0xffffff,"%d",c);
}

