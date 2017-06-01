#pragma once

#include "task.hpp"

typedef struct Window {
	SHEET *sheet;
	Task *task;
	struct Window *prev, *next;
} Window;

void dwm_addtop(SHEET *sht, Task *task);
void dwm_movetop(Window *w);
void dwm_init(SHEET *sht);
void dwm_refresh(Window *old,Window *neww);
void dwm_removewindow(SHEET *sht);
void dwm_mousepressed(int x, int y);
void dwm_mousedragged(int dx, int dy);
void dwm_mousereleased();
