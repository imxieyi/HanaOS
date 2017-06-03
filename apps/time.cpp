#include <stdint.h>
#include "rtc.hpp"
#include "apps.hpp"
#include "apps_api.hpp"

void *app_time(char *buffer, uint32_t *cbuffer, char *param) {
	STDOUT out(buffer,cbuffer);
	auto time=rtc_time();
	char timestr[64];
	time2str(&time,timestr);
	out.printf(0xceff85,"%s\n",timestr);
}
