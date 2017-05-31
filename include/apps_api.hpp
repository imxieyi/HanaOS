#pragma once

#include <stdint.h>

class STDOUT {
private:
	char *buffer;
	uint32_t *cbuffer;
	int offset;
	void append(char *str, uint32_t color);
public:
	STDOUT(char *buffer, uint32_t *cbuffer);
	int printf(uint32_t color, const char *format, ...);
};

int initAcpi(void);
void acpiPowerOff(void);
