#pragma once
#include <stdint.h>
#include "multiboot.h"

#define VGA_RGBPACK(r, g, b, a) ((a << 24)|(r << 16)|(g << 8)|(b << 0))
void init_graphics(vbe_mode_info_t *vbeinfo);

class GRAPHICS {
private:
	uint8_t bpp;
	uint16_t fb_stride32;
	uint32_t color;
	char title[32];
	void draw_closebtn();
public:
	uint16_t width,height;
	uint32_t *vram;
	uint32_t bgcolor;
	void init(uint32_t *vram,uint16_t width,uint16_t height,uint8_t bpp,uint16_t fb_stride32);
	void init_screen();
	void show_bgimg();
	void init_mouse_cursor();
	void init_window(const char *title);
	void setcolor(uint8_t r,uint8_t g,uint8_t b,uint8_t a);
	void setcolor(uint8_t r,uint8_t g,uint8_t b);
	void setcolor(uint32_t c);
	void setcolor(uint32_t c,bool withalpha);
	void boxfill(int x0, int y0, int x1, int y1);
	void putchar(unsigned char ch,int scale,int x,int y);
	void putstr(const char *str,int scale,int x,int y);
	void putblock8_8(int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
	void make_textbox(int x0, int y0,int x1, int y1, uint32_t bgcolor);
	void make_textbox(int x0, int y0,int x1, int y1, uint32_t bgcolor, bool withalpha);
	void window_inactive();
	void window_active();
};
