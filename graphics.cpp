#include <stdint.h>
#include "multiboot.h"
#include "graphics.hpp"
#include "bgimg.hpp"
#include "mousecursor.hpp"
#include "hankaku.hpp"
#include "hanastd.hpp"
using namespace hanastd;

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

void GRAPHICS::setcolor(uint32_t c,bool withalpha){
	color=c;
}

void GRAPHICS::boxfill(int x0,int y0,int x1,int y1){
	for(int y=y0;y<=y1;y++)
		for(int x=x0;x<=x1;x++)
			vram[y*width+x]=color;
}

void GRAPHICS::putchar(unsigned char ch,int scale,int x,int y){
	for(int i=0;i<16;i++){
		uint8_t bit=0x80;
		for(int j=0;j<8;j++){
			uint8_t b=hankaku[ch*16+i];
			if((b&bit)!=0)boxfill(x+j*scale,y+i*scale,x+(j+1)*scale-1,y+(i+1)*scale-1);
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
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++){
            pixel=bgimg[(y*1920+x)%(1920*1080)];
            r=(pixel&0xff0000)>>16;
            g=(pixel&0xff00)>>8;
            b=pixel&0xff;
			vram[y*width+x]=VGA_RGBPACK(r,g,b,0xff);
        }

}

void GRAPHICS::draw_closebtn(){
	for(int i=0;i<14;i++){
		//Box
		vram[(6+i)*width+(width-20)]=0xff000000;
		vram[6*width+(width-20+i)]=0xff000000;
		vram[(19-i)*width+(width-7)]=0xff000000;
		vram[19*width+(width-20+i)]=0xff000000;
		//X Symbol
		vram[(6+i)*width+(width-20+i)]=0xff000000;
		vram[(19-i)*width+(width-20+i)]=0xff000000;
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
	putstr(title,1,6,6);
	setcolor(0,0,0);
	putstr(title,1,5,5);
	strncpy(title,this->title,strlen(title));
	draw_closebtn();
}

void GRAPHICS::window_inactive(){
	setcolor(0xd6,0xeb,0xff,0xcc);
	boxfill(1,1,width-2,24);
	boxfill(1,25,2,height-2);
	boxfill(width-3,25,width-2,height-2);
	boxfill(3,height-3,width-3,height-2);
	setcolor(255,255,255);
	putstr(title,1,6,6);
	setcolor(0,0,0);
	putstr(title,1,5,5);
	draw_closebtn();
}

void GRAPHICS::window_active(){
	setcolor(0x99,0xcc,0xff,0xcc);
	boxfill(1,1,width-2,24);
	boxfill(1,25,2,height-2);
	boxfill(width-3,25,width-2,height-2);
	boxfill(3,height-3,width-3,height-2);
	setcolor(255,255,255);
	putstr(title,1,6,6);
	setcolor(0,0,0);
	putstr(title,1,5,5);
	draw_closebtn();
}

void GRAPHICS::init_mouse_cursor(){
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
			vram[y*width+x]=mouse_platte[cursor[y+x*height]];
}

void GRAPHICS::make_textbox(int x0, int y0,int x1, int y1, uint32_t bgcolor){
	setcolor(0x848484);
	boxfill(x0-2, y0-3, x1+1, y0-3);
	boxfill(x0-3, y0-3, x0-3, y1+1);
	setcolor(0xffffff);
	boxfill(x0-3, y1+2, x1+1, y1+2);
	boxfill(x1+2, y0-3, x1+2, y1+2);
	setcolor(0x000000);
	boxfill(x0-1, y0-2, x1+0, y0-2);
	boxfill(x0-2, y0-2, x0-2, y1+0);
	setcolor(0xc6c6c6);
	boxfill(x0-2, y1+1, x1+0, y1+1);
	boxfill(x1+1, y0-2, x1+1, y1+1);
	setcolor(bgcolor);
	boxfill(x0-1, y0-1, x1+0, y1+0);
}

void GRAPHICS::make_textbox(int x0, int y0,int x1, int y1, uint32_t bgcolor, bool withalpha){
	setcolor(0x848484);
	boxfill(x0-2, y0-3, x1+1, y0-3);
	boxfill(x0-3, y0-3, x0-3, y1+1);
	setcolor(0xffffff);
	boxfill(x0-3, y1+2, x1+1, y1+2);
	boxfill(x1+2, y0-3, x1+2, y1+2);
	setcolor(0x000000);
	boxfill(x0-1, y0-2, x1+0, y0-2);
	boxfill(x0-2, y0-2, x0-2, y1+0);
	setcolor(0xc6c6c6);
	boxfill(x0-2, y1+1, x1+0, y1+1);
	boxfill(x1+1, y0-2, x1+1, y1+1);
	setcolor(bgcolor,withalpha);
	boxfill(x0-1, y0-1, x1+0, y1+0);
}
