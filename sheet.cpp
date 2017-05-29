#include "asmfunc.hpp"
#include "hanastd.hpp"
#include "heap.hpp"
#include "graphics.hpp"
#include "sheet.hpp"
using namespace hanastd;

//图层控制
SHEETCTRL *sheetctrl_init(class MEMMAN *memman,vbe_mode_info_t *vbeinfo){
	int i;
	SHEETCTRL *ctl=(SHEETCTRL *)memman->alloc_4k(sizeof(SHEETCTRL));
	if(ctl==0){
		return 0;
	}
	ctl->setmemman(memman);
	ctl->vram=(uint32_t*)vbeinfo->physbase;
	ctl->xsize=vbeinfo->Xres;
	ctl->ysize=vbeinfo->Yres;
	ctl->fb_stride32=vbeinfo->pitch/sizeof(uint32_t);
	ctl->top=-1;
	for(i=0;i<MAX_SHEETS;i++){
		ctl->sheets0[i].flags=0;//未使用
		ctl->sheets0[i].sctrl=ctl;
		ctl->sheets[i]=NULL;
	}
	return ctl;
}

SHEET *SHEETCTRL::allocsheet(int xsize,int ysize){
	int i;
	for(i=0;i<MAX_SHEETS;i++)
		if(!(sheets0[i].flags)){
			SHEET *sht=&sheets0[i];
			sht->graphics=(GRAPHICS *)memman->alloc_4k(sizeof(GRAPHICS));
			uint32_t *buf=(uint32_t *)memman->alloc_4k(xsize*ysize*4);
			memset(buf,0,xsize*ysize*4);
			if(buf==0){
				return 0;
			}
			sht->graphics->init(buf,xsize,ysize,bpp,fb_stride32);
			sht->flags=SHEET_USE;
			sht->height=-1;
			sht->bxsize=xsize;
			sht->bysize=ysize;
			return sht;
		}
	return 0;//无空闲图层
}

void SHEETCTRL::setmemman(class MEMMAN *memman){
	this->memman=memman;
}

void SHEET::updown(int height){
	if(sctrl->top<height)sctrl->top=height;
	if(this->height!=-1)
		sctrl->sheets[this->height]=NULL;
	this->height=height;
	sctrl->sheets[height]=this;
	sctrl->refreshall(vx0,vy0,vx0+bxsize-1,vy0+bysize-1);
	return;
}

void SHEETCTRL::refreshpixel(int x, int y, int height){
	if(height<0)return;
	SHEET *sht=sheets[height];
	if(sht==NULL){
		goto next;
	}else if(!sht->flags){
		goto next;
	}else{
		if(x<sht->vx0||x>sht->vx0+sht->bxsize-1||y<sht->vy0||y>sht->vy0+sht->bysize-1)
			goto next;
		int vx=x-sht->vx0;
		int vy=y-sht->vy0;
		uint32_t bufcolor=sht->graphics->vram[vy * sht->bxsize + vx];
		uint16_t alpha=(bufcolor>>24)&0xff;
		if(alpha==0)goto next;
		uint32_t newcolor=bufcolor;
		if(alpha<0xff){
			refreshpixel(x,y,height-1);
			uint16_t bufr=(bufcolor>>16)&0xff;
			uint16_t bufg=(bufcolor>>8)&0xff;
			uint16_t bufb=bufcolor&0xff;
			uint32_t oldcolor=vram[y * fb_stride32 + x];
			uint16_t oldr=(oldcolor>>16)&0xff;
			uint16_t oldg=(oldcolor>>8)&0xff;
			uint16_t oldb=oldcolor&0xff;
			uint8_t blend=0xff-alpha;
			newcolor=VGA_RGBPACK((oldr*blend+bufr*alpha)>>8,(oldg*blend+bufg*alpha)>>8,(oldb*blend+bufb*alpha)>>8,0xff);
		}
		vram[y * fb_stride32 + x] = newcolor;
		return;
	}
	next:refreshpixel(x,y,height-1);
}

void SHEETCTRL::refreshall(int vx0, int vy0, int vx1, int vy1){
	vx0=vx0<0?0:vx0;
	vy0=vy0<0?0:vy0;
	vx1=vx1>=xsize?(xsize-1):vx1;
	vy1=vy1>=ysize?(ysize-1):vy1;
	for(int y=vy0;y<=vy1;y++)
		for(int x=vx0;x<=vx1;x++)
			refreshpixel(x,y,top);
	return;
}

void SHEET::slide(int vx0,int vy0){
	int old_vx0 = this->vx0, old_vy0 = this->vy0;
	this->vx0 = vx0;
	this->vy0 = vy0;
	if (height >= 0) {
		sctrl->refreshall(old_vx0, old_vy0, old_vx0 + bxsize-1, old_vy0 + bysize-1);
		sctrl->refreshall(vx0, vy0, vx0 + bxsize-1, vy0 + bysize-1);
	}
	return;
}

void SHEET::putstring(int x,int y,int scale,uint32_t f,uint32_t b,char *s){
	int l=strlen(s);
	graphics->setcolor(b);
	graphics->boxfill(x,y,x+l*8*scale,y+16*scale);
	graphics->setcolor(f);
	graphics->putstr(s,scale,x,y);
	if(height>=0)
		sctrl->refreshall(x+vx0,y+vy0,x+vx0+l*8*scale,y+vy0+16*scale);
	return;
}

void SHEET::putstring(int x,int y,int scale,uint32_t f,uint32_t b,bool withalpha,char *s){
	int l=strlen(s);
	graphics->setcolor(b,true);
	graphics->boxfill(x,y,x+l*8*scale,y+16*scale);
	graphics->setcolor(f);
	graphics->putstr(s,scale,x,y);
	if(height>=0)
		sctrl->refreshall(x+vx0,y+vy0,x+vx0+l*8*scale,y+vy0+16*scale);
	return;
}

void SHEET::refresh(int bx0,int by0,int bx1,int by1){
	sctrl->refreshall(vx0+bx0,vy0+by0,vx0+bx1,vy0+by1);
}

void SHEET::free(){
	if(height>=0)
		updown(-1);
	flags=0;
	return;
}
