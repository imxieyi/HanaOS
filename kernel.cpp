#include <stddef.h>
#include <stdint.h>
#include "asmfunc.hpp"
#include "multiboot.h"
#include "graphics.hpp"

#ifdef __cplusplus
extern "C"{
#endif
void kernel_main(multiboot_info_t *hdr,uint32_t magic)
{
	int eflags=io_load_eflags();
	io_cli();
	init_graphics((vbe_mode_info_t*)hdr->vbe_mode_info);
    show_bgimg();
	setcolor(0x66ccff);
	boxfill(0,100,500,300);
	setcolor(0xff0000);
	putstr("ABC 123",2,50,110);
	putstr("Hello HanaOS!",2,50,150);
	io_store_eflags(eflags);
	for(;;)
		io_hlt();
}
#ifdef __cplusplus
}
#endif
