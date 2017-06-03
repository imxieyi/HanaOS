//http://forum.osdev.org/viewtopic.php?t=17433&p=142528
#include <stdint.h>
#include "asmfunc.hpp"
#include "isr.hpp"
#include "rtc.hpp"
#include "hanastd.hpp"
using namespace hanastd;

time_t global_time;
bool bcd,timeset=false;

uint8_t read_register(uint8_t reg){
	io_out8(0x70,reg);
	return io_in8(0x71);
}

void write_register(uint8_t reg,uint8_t value){
	io_out8(0x70,reg);
	io_out8(0x71,value);
}

uint8_t bcd2bin(uint8_t bcd){
	return ((bcd>>4)*10)+(bcd&0x0f);
}

const int dayspermonth[13]={
	0,31,28,31,30,31,30,31,31,30,31,30,31};

const int dayspermonth_leap[13]={
	0,31,29,31,30,31,30,31,31,30,31,30,31};

bool isleapyear(time_t *time){
	if(time->year%4==0)
		if(time->century%4!=0)
			return true;
	return false;
}

int daysofamonth(time_t *time,int month){
	if(isleapyear(time))
		return dayspermonth_leap[month];
	return dayspermonth[month];
}

void localize_time(time_t *time,int timezone){
	time->hour+=timezone;
	if(time->hour<0){
		time->hour+=24;
		time->day--;
		time->weekday--;
		if(time->weekday==0)
			time->weekday=7;
		if(time->day<=0){
			time->month--;
			if(time->month<=0){
				time->month=1;
				time->year--;
				if(time->year<0){
					time->year=99;
					time->century--;
				}
			}
			time->day=daysofamonth(time,time->month);
		}
	}
	if(time->hour>=24){
		time->hour-=24;
		time->day++;
		time->weekday++;
		if(time->weekday==7)
			time->weekday=1;
		if(time->day>daysofamonth(time,time->month)){
			time->day=1;
			time->month++;
			if(time->month>12){
				time->month=1;
				time->year++;
				if(time->year>99){
					time->year=0;
					time->century++;
				}
			}
		}
	}
}

const char week_map[8][10]={
	"","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
};

void time2str(time_t *time,char *buffer){
	sprintf(buffer,"%2d%02d-%02d-%02d %s %02d:%02d:%02d %s",
			   time->century,time->year,time->month,time->day,week_map[time->weekday],
			   time->hour,time->minute,time->second,TIMEZONE_DESCRIPTION);
}

time_t rtc_time(){
	return {
		global_time.second,
		global_time.minute,
		global_time.hour,
		global_time.weekday,
		global_time.day,
		global_time.month,
		global_time.year,
		global_time.century
	};
}

void rtc_handler(registers_t regs){
	if(read_register(0x0c)&0x10){
		timeset=true;
		if(bcd){
			global_time.second =bcd2bin(read_register(0x00));
			global_time.minute =bcd2bin(read_register(0x02));
			global_time.hour   =bcd2bin(read_register(0x04));
			global_time.weekday=bcd2bin(read_register(0x06));
			global_time.day    =bcd2bin(read_register(0x07));
			global_time.month  =bcd2bin(read_register(0x08));
			global_time.year   =bcd2bin(read_register(0x09));
			global_time.century=bcd2bin(read_register(0x32));
		}else{
			global_time.second =read_register(0x00);
			global_time.minute =read_register(0x02);
			global_time.hour   =read_register(0x04);
			global_time.weekday=read_register(0x06);
			global_time.day    =read_register(0x07);
			global_time.month  =read_register(0x08);
			global_time.year   =read_register(0x09);
			global_time.century=read_register(0x32);
		}
		//https://zh.wikipedia.org/wiki/%E6%98%9F%E6%9C%9F%E7%9A%84%E8%A8%88%E7%AE%97
		int cc=2*(3-(global_time.century%4));
		int yy=(global_time.year&28+(global_time.year%28)/4)%7;
		int mm=((34+(global_time.month-3)%12*26)/10)%7;
		int dd=global_time.day%7;
		global_time.weekday=(cc+yy+mm+dd)%7+1;
		localize_time(&global_time,TIMEZONE);
	}
}

void init_rtc(){
	uint8_t status;
	status=read_register(0x0b);
	status|=0x02;
	status|=0x10;
	status&=~0x20;
	status&=~0x40;
	bcd=!(status&0x04);
	write_register(0x0b,status);
	read_register(0x0c);
	register_interrupt_handler(IRQ8,&rtc_handler);
}
