#ifndef _GRAPHICS_HPP
#define _GRAPHICS_HPP
#include <stdint.h>
#include "multiboot.h"

#define VGA_RGBPACK(r, g, b) ((r << 16)|(g << 8)|(b << 0))
void setcolor(uint8_t r,uint8_t g,uint8_t b);
void setcolor(uint32_t c);
void boxfill(int x0,int y0,int x1,int y1);
void putchar(unsigned char ch,int scale,int x,int y);
void putstr(const char *str,int scale,int x,int y);
void show_bgimg();
void init_graphics(vbe_mode_info_t *vbeinfo);

#endif
