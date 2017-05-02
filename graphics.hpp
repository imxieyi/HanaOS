#ifndef _GRAPHICS_HPP
#define _GRAPHICS_HPP

#define VGA_RGBPACK(r, g, b) ((r << 16)|(g << 8)|(b << 0))
void show_bgimg();
void init_graphics(vbe_mode_info_t *vbeinfo);

#endif
