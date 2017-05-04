#include <stdint.h>
#include "multiboot.h"
#include "graphics.hpp"
#include "bgimg.hpp"
#include "mousecursor.hpp"
#include "hankaku.hpp"

void GRAPHICS::init(uint32_t *vram,uint16_t width,uint16_t height,uint8_t bpp,uint16_t fb_stride32){
	this->vram=vram;
	this->width=width;
	this->height=height;
	this->bpp=bpp;
	this->fb_stride32=fb_stride32;
}

void GRAPHICS::setcolor(uint8_t r,uint8_t g,uint8_t b,uint8_t a){
	color=VGA_RGBPACK(r,g,b,a);
}

void GRAPHICS::setcolor(uint8_t r,uint8_t g,uint8_t b){
	color=VGA_RGBPACK(r,g,b,0xff);
}

void GRAPHICS::setcolor(uint32_t c){
	color=0xff000000|c;
}

void GRAPHICS::boxfill(int x0,int y0,int x1,int y1){
	for(int x=x0;x<=x1;x++)
		for(int y=y0;y<=y1;y++)
//			vram[y*fb_stride32+x]=color;
			vram[y*width+x]=color;
}

void GRAPHICS::putchar(unsigned char ch,int scale,int x,int y){
	for(int i=0;i<16;i++){
		uint8_t bit=0x80;
		for(int j=0;j<8;j++){
			uint8_t b=hankaku[ch*16+i];
			if((b&bit)!=0)boxfill(x+j*scale,y+i*scale,x+(j+1)*scale,y+(i+1)*scale);
			bit>>=1;
		}
	}
}

void GRAPHICS::putstr(const char *str,int scale,int x,int y){
	for(int offset=0;*(str+offset)!=0;offset++)
		putchar(*(str+offset),scale,x+8*scale*offset,y);
}

void GRAPHICS::show_bgimg(){
    uint32_t pixel=0;
    uint8_t r,g,b;
	for(int x=0;x<width;x++)
		for(int y=0;y<height;y++){
            pixel=bgimg[y+x*768];
            r=(pixel&0xff0000)>>16;
            g=(pixel&0xff00)>>8;
            b=pixel&0xff;
			vram[y*width+x]=VGA_RGBPACK(r,g,b,0xff);
        }
}

void GRAPHICS::init_window(const char *title){
	setcolor(0,0,0);
	boxfill(0,0,width-1,height-1);
	setcolor(0x99,0xcc,0xff,0xcc);
	boxfill(1,1,width-2,height-2);
	setcolor(192,192,192);
	bgcolor=color;
	boxfill(3,25,width-4,height-4);
	setcolor(255,255,255);
	putstr(title,1,4,6);
	setcolor(0,0,0);
	putstr(title,1,3,5);
}

void GRAPHICS::init_mouse_cursor(){
    uint32_t pixel=0;
    uint8_t r,g,b,a;
	for(int x=0;x<width;x++)
		for(int y=0;y<height;y++){
            pixel=cursor[y+x*height];
            r=(pixel&0xff0000)>>16;
            g=(pixel&0xff00)>>8;
            b=pixel&0xff;
			a=0xff;
			if(r==0xff&&g==0x00&&b==0x00)a=0;
			vram[y*width+x]=VGA_RGBPACK(r,g,b,a);
        }
}
