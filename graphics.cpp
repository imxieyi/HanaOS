#include "multiboot.h"
#include "graphics.hpp"
#include <stdint.h>
#include "bgimg.hpp"

uint16_t width,height;
uint8_t bpp;
uint8_t *vram;
uint16_t fb_stride,fb_stride32;

void show_bgimg(){
    uint32_t pixel=0;
    uint8_t r,g,b;
	for(int x=0;x<width;x++)
		for(int y=0;y<height;y++){
            pixel=bgimg[y+x*768];
            r=(pixel&0xff0000)>>16;
            g=(pixel&0xff00)>>8;
            b=pixel&0xff;
			((uint32_t*)vram)[y*fb_stride32+x]=VGA_RGBPACK(r,g,b);
        }
}

void init_graphics(vbe_mode_info_t *vbeinfo){
	width=vbeinfo->Xres;
	height=vbeinfo->Yres;
	bpp=vbeinfo->bpp/8;
	vram=(uint8_t*)vbeinfo->physbase;
	fb_stride=vbeinfo->pitch;
	fb_stride32=fb_stride/sizeof(uint32_t);
}
