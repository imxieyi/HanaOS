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
	init_graphics((vbe_mode_info_t*)hdr->vbe_mode_info);
    show_bgimg();
	for(;;)
		io_hlt();
}
#ifdef __cplusplus
}
#endif
