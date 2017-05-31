#pragma once
#include <stdint.h>

typedef struct {
	char name[32];
	uintptr_t entry;
}App;

#define MAX_APPS 100

class AppStore {
private:
	App apps[100];
	int pub_offset=0;
public:
	void publish(const char *name, uintptr_t entry);
	uintptr_t search(const char *name);
};
