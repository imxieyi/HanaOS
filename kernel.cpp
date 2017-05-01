#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "bgimg.hpp"
//xorriso,mtools

#define VGA_RGBPACK(r, g, b) ((r << 16)|(g << 8)|(b << 0))
#ifdef __cplusplus
extern "C"{
#endif
void kernel_main(unsigned long addr,uint32_t magic)
{
	multiboot_info_t *hdr=(multiboot_info_t*)addr;
	vbe_mode_info_t *vbeinfo=(vbe_mode_info_t*)hdr->vbe_mode_info;
	uint16_t width=vbeinfo->Xres;
	uint16_t height=vbeinfo->Yres;
	uint8_t bpp=vbeinfo->bpp/8;
	uint8_t *vram=(uint8_t*)vbeinfo->physbase;
	uint16_t fb_stride=vbeinfo->pitch;
	uint16_t fb_stride32=fb_stride/sizeof(uint32_t);
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
	for(;;)
		io_hlt();
}
#ifdef __cplusplus
}
#endif
