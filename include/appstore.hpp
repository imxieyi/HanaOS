#pragma once
#include <stdint.h>

typedef void* (* app_func_t)(char *, char *);

typedef struct {
	char name[32];
	app_func_t entry;
	bool newtask=false;
}App;

void appstore_init();
void appstore_publish(const char *name, app_func_t entry);
app_func_t appstore_search(const char *name);
