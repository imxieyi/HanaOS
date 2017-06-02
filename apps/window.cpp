#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "asmfunc.hpp"
#include "task.hpp"
#include "graphics.hpp"
#include "fifo.hpp"
#include "apps.hpp"
#include "apps_api.hpp"
#include "dwm.hpp"
using namespace hanastd;

void task_window(void *arg) {
	SHEET *sht=init_window(200,100,"Window Test");
	int count=0;
	char str[32];
	for(;;){
		count++;
		sprintf(str,"%d",count);
		sht->putstring(8,40,1,0,sht->graphics->bgcolor,str);
	}
}

void *app_window(char *buffer, uint32_t *cbuffer, char *param) {
	auto task=createTask("window",&task_window,NULL);
	task_run(task,2,1);
}
