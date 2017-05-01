#include <stdint.h>
#include "multiboot.h"
#include "graphics.hpp"
#include "bgimg.hpp"
#include "hankaku.hpp"

uint16_t width,height;
uint8_t bpp;
uint32_t *vram;
uint16_t fb_stride,fb_stride32;
uint32_t color;

void setcolor(uint8_t r,uint8_t g,uint8_t b){
	color=VGA_RGBPACK(r,g,b);
}
void setcolor(uint32_t c){
	color=c;
}

void boxfill(int x0,int y0,int x1,int y1){
	for(int x=x0;x<=x1;x++)
		for(int y=y0;y<=y1;y++)
			vram[y*fb_stride32+x]=color;
}

void putchar(unsigned char ch,int scale,int x,int y){
	for(int i=0;i<16;i++){
		uint8_t bit=0x80;
		for(int j=0;j<8;j++){
			uint8_t b=hankaku[ch*16+i];
			if((b&bit)!=0)boxfill(x+j*scale,y+i*scale,x+(j+1)*scale,y+(i+1)*scale);
			bit>>=1;
		}
	}
}

void putstr(const char *str,int scale,int x,int y){
	for(int offset=0;*(str+offset)!=0;offset++)
		putchar(*(str+offset),scale,x+8*scale*offset,y);
}

void show_bgimg(){
    uint32_t pixel=0;
    uint8_t r,g,b;
	for(int x=0;x<width;x++)
		for(int y=0;y<height;y++){
            pixel=bgimg[y+x*768];
            r=(pixel&0xff0000)>>16;
            g=(pixel&0xff00)>>8;
            b=pixel&0xff;
			vram[y*fb_stride32+x]=VGA_RGBPACK(r,g,b);
        }
}

void init_graphics(vbe_mode_info_t *vbeinfo){
	width=vbeinfo->Xres;
	height=vbeinfo->Yres;
	bpp=vbeinfo->bpp/8;
	vram=(uint32_t*)vbeinfo->physbase;
	fb_stride=vbeinfo->pitch;
	fb_stride32=fb_stride/sizeof(uint32_t);
}