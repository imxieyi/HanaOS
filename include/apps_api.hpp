#pragma once

#include <stdint.h>
#include "sheet.hpp"

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

SHEET *init_window(int width, int height, char *title);
void close_window(SHEET *sht);

int initAcpi(void);
void acpiPowerOff(void);
