#pragma once

#define MAX_SHEETS		256
#define SHEET_USE		1
class SHEETCTRL;
class SHEET {
public:
	SHEETCTRL *sctrl;
	class GRAPHICS *graphics;
	int bxsize,bysize,vx0,vy0,height,flags;//height越大越上层
	SHEET(SHEETCTRL *ctl);
	void updown(int height);
	void refresh(int bx0,int by0,int bx1,int by1);
	void slide(int vx0,int vy0);
	void putstring(int x,int y,int scale,uint32_t f,uint32_t b,char *s);
	void putstring(int x,int y,int scale,uint32_t f,uint32_t b,bool withalpha,char *s);
	void free();
	void window_inactive();
	void window_active();
};
class SHEETCTRL {
private:
	class MEMMAN *memman;
	uint8_t bpp;
	void refreshpixel(int x, int y, int height);
public:
	uint32_t *vram;
	uint16_t fb_stride32;
	int xsize,ysize,top;
	SHEET *sheets[MAX_SHEETS];
	SHEET sheets0[MAX_SHEETS];
	SHEET *allocsheet(int xsize,int ysize);
	void setmemman(class MEMMAN *memman);
	void refreshall(int vx0, int vy0, int vx1, int vy1);
};
SHEETCTRL *sheetctrl_init(class MEMMAN *memman,vbe_mode_info_t *vbeinfo);
