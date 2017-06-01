#include <stdint.h>
#include <stddef.h>
#include "sheet.hpp"
#include "dwm.hpp"
#include "heap.hpp"

Window *top=NULL;
extern MEMMAN *memman;
extern SHEET *mouse_sht;

void dwm_init(SHEET *sht){
	top=(Window*)memman->alloc(sizeof(Window));
	top->sheet=sht;
	top->prev=NULL;
	top->next=NULL;
}

void dwm_addtop(SHEET *sht){
	auto w=(Window*)memman->alloc(sizeof(Window));
	w->prev=NULL;
	w->next=top;
	w->sheet=sht;
	if(top!=NULL)top->prev=w;
	top=w;
	mouse_sht->updown(mouse_sht->height+1);
	sht->updown(mouse_sht->height-1);
	dwm_refresh(top->next,top);
}

void dwm_movetop(Window *w){
	int topheight=top->sheet->height;
	//w->sheet->updown(-1);
	for(Window *i=w->prev;i!=top&&i!=NULL;i=i->prev)
		i->sheet->updown(i->sheet->height-1);
	top->sheet->updown(topheight-1);
	if(w->next!=NULL)
		w->next->prev=w->prev;
	if(w->prev!=NULL)
		w->prev->next=w->next;
	dwm_refresh(top,w);
	top->prev=w;
	w->prev=NULL;
	w->next=top;
	top=w;
	w->sheet->updown(topheight);
}

void dwm_removewindow(SHEET *sht){
	sht->free();
	Window *i;
	for(i=top;i->sheet!=sht;i=i->next)
		i->sheet->updown(i->sheet->height-1);
	if(sht==top->sheet){
		dwm_refresh(NULL,top->next);
		top=top->next;
		top->prev=NULL;
	} else {
		i->prev->next=i->next;
		if(i->next!=NULL)
			i->next->prev=i->prev;
	}
	mouse_sht->updown(mouse_sht->height-1);
	memman->free((uintptr_t)i,sizeof(Window));
}

bool ontitlebar=false;

void dwm_mousepressed(int x, int y){
	int x0=top->sheet->vx0;
	int y0=top->sheet->vy0;
	int w=top->sheet->bxsize;
	int h=top->sheet->bysize;
	if(x>=x0&&y>=y0&&x<=x0+w&&y<=y0+h){
		if(y<=y0+24)
			ontitlebar=true;
		return;
	}
	for(Window *i=top->next;i!=NULL;i=i->next){
		x0=i->sheet->vx0;
		y0=i->sheet->vy0;
		w=i->sheet->bxsize;
		h=i->sheet->bysize;
		if(x>=x0&&y>=y0&&x<=x0+w&&y<=y0+h){
			dwm_movetop(i);
			if(y<=y0+24)
				ontitlebar=true;
			return;
		}
	}
}

void dwm_mousedragged(int dx, int dy){
	if(ontitlebar)
		top->sheet->slide(top->sheet->vx0+dx,top->sheet->vy0+dy);
}

void dwm_mousereleased(){
	ontitlebar=false;
}

void dwm_refresh(Window *old,Window *neww){
	if(old!=NULL)
		old->sheet->window_inactive();
	neww->sheet->window_active();
}
