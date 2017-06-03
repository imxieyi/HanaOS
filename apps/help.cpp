#include <stdint.h>
#include "apps.hpp"
#include "appstore.hpp"
#include "apps_api.hpp"
#include "hanastd.hpp"
using namespace hanastd;

extern App *apps;
extern int app_count;

#define min(a,b) ((a)<(b)?(a):(b))

void *app_help(char *buffer, uint32_t *cbuffer, char *param) {
	STDOUT out(buffer,cbuffer);
	char names[app_count][16];
	for(int i=0;i<app_count;i++)
		strncpy(apps[i].name,names[i],strlen(apps[i].name));
	char t[16];
	int il,jl;
	for(int i=0;i<app_count;i++)
		for(int j=i+1;j<app_count;j++){
			il=strlen(names[i])-1;
			jl=strlen(names[j])-1;
			for(int k=0;(k<min(il,jl))&&(names[i][k]>=names[j][k]);k++){
				if(names[i][k]>names[j][k]){
					strncpy(names[i],t,il);
					strncpy(names[j],names[i],jl);
					strncpy(t,names[j],il);
					break;
				}
			}
		}
	out.printf(0xffb8aa,"List of %d Apps:\n",app_count);
	for(int i=0;i<app_count;i++){
		out.printf(0xdfc2ff,"%s\n",names[i]);
	}
}

