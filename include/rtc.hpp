#pragma once

#define TIMEZONE +8
#define TIMEZONE_DESCRIPTION "CST"

typedef struct {
	uint8_t second,minute,hour,weekday,day,month,year,century;
} time_t;

void time2str(time_t *time,char *buffer);
time_t rtc_time();
void init_rtc();
