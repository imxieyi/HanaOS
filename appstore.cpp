#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "appstore.hpp"
#include "apps.hpp"
#include "heap.hpp"
using namespace hanastd;

App *apps;
int app_count=0;
extern MEMMAN *memman;

#define MAX_APPS 100

void appstore_init(){
	apps=(App*)memman->alloc_4k(MAX_APPS*sizeof(App));
	publish_all();
}

void appstore_publish(const char *name, app_func_t entry){
	apps[app_count].entry=entry;
	strncpy(name,apps[app_count].name,strlen(name));
	app_count++;
}

app_func_t appstore_search(const char *name){
	for(int i=0;i<app_count;i++)
		if(strncmp(name,apps[i].name,strlen(name)))
			return apps[i].entry;
	return 0;
}
