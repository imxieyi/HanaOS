#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "apps_api.hpp"
#include <string.h>

void *app_poweroff(char *buffer, uint32_t *cbuffer, char *param) {
	int i=initAcpi();
	if(i==0){
		acpiPowerOff();
	}else{
		STDOUT out(buffer,cbuffer);
		out.printf(0xff99eb,"ACPI init failed: ");
		switch(i){
		case -1:
			out.printf(0xfdffc4,"no ACPI\n");
			break;
		case 1:
			out.printf(0xfdffc4,"S5 parse error\n");
			break;
		case 2:
			out.printf(0xfdffc4,"S5 not present\n");
			break;
		case 3:
			out.printf(0xfdffc4,"DSDT invalid\n");
			break;
		case 4:
			out.printf(0xfdffc4,"no valid FACP present\n");
			break;
		}
	}
}
