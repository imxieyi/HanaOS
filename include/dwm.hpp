#pragma once

typedef struct Window {
	SHEET *sheet;
	struct Window *prev, *next;
} Window;

void dwm_addtop(SHEET *sht);
void dwm_movetop(Window *w);
void dwm_init(SHEET *sht);
void dwm_refresh(Window *old,Window *neww);
void dwm_removewindow(SHEET *sht);
void dwm_mousepressed(int x, int y);
void dwm_mousedragged(int dx, int dy);
void dwm_mousereleased();
