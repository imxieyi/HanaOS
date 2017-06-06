#include "asmfunc.hpp"
#include "hanastd.hpp"
#include "heap.hpp"
#include "graphics.hpp"
#include "sheet.hpp"
using namespace hanastd;

SHEETCTRL *shtctl;
int screen_width,screen_height;

//图层控制
void shtctl_init(vbe_mode_info_t *vbeinfo){
	int i;
	shtctl=(SHEETCTRL *)malloc(sizeof(SHEETCTRL));
	shtctl->vram=(uint32_t*)vbeinfo->physbase;
	screen_width=vbeinfo->Xres;
	screen_height=vbeinfo->Yres;
	shtctl->fb_stride32=vbeinfo->pitch/sizeof(uint32_t);
	shtctl->top=-1;
	for(i=0;i<MAX_SHEETS;i++){
		shtctl->sheets0[i].flags=0;//未使用
		shtctl->sheets[i]=NULL;
	}
}

SHEET *shtctl_alloc(int xsize,int ysize){
	return shtctl->allocsheet(xsize,ysize);
}

SHEET *SHEETCTRL::allocsheet(int xsize,int ysize){
	int i;
	for(i=0;i<MAX_SHEETS;i++)
		if(!(sheets0[i].flags)){
			SHEET *sht=&sheets0[i];
			sht->graphics=(GRAPHICS *)malloc(sizeof(GRAPHICS));
			uint32_t *buf=(uint32_t *)malloc(xsize*ysize*4);
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

void SHEET::updown(int height){
	if(height==-1){
		shtctl->sheets[this->height]=NULL;
		this->height=-1;
		shtctl_refresh(vx0,vy0,vx0+bxsize-1,vy0+bysize-1);
		return;
	}
	if(shtctl->top<height)shtctl->top=height;
	if(this->height!=-1&&shtctl->sheets[this->height]==this)
		shtctl->sheets[this->height]=NULL;
	this->height=height;
	shtctl->sheets[height]=this;
	shtctl_refresh(vx0,vy0,vx0+bxsize-1,vy0+bysize-1);
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

void shtctl_refresh(int vx0,int vy0,int vx1,int vy1){
	shtctl->refreshall(vx0,vy0,vx1,vy1);
}

void SHEETCTRL::refreshall(int vx0, int vy0, int vx1, int vy1){
	vx0=vx0<0?0:vx0;
	vy0=vy0<0?0:vy0;
	vx1=vx1>=screen_width?(screen_width-1):vx1;
	vy1=vy1>=screen_height?(screen_height-1):vy1;
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
		shtctl_refresh(old_vx0, old_vy0, old_vx0 + bxsize-1, old_vy0 + bysize-1);
		shtctl_refresh(vx0, vy0, vx0 + bxsize-1, vy0 + bysize-1);
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
		shtctl_refresh(x+vx0,y+vy0,x+vx0+l*8*scale,y+vy0+16*scale);
	return;
}

void SHEET::putstring(int x,int y,int scale,uint32_t f,uint32_t b,bool withalpha,char *s){
	int l=strlen(s);
	graphics->setcolor(b,true);
	graphics->boxfill(x,y,x+l*8*scale,y+16*scale);
	graphics->setcolor(f);
	graphics->putstr(s,scale,x,y);
	if(height>=0)
		shtctl_refresh(x+vx0,y+vy0,x+vx0+l*8*scale,y+vy0+16*scale);
	return;
}
void SHEET::window_inactive(){
	graphics->window_inactive();
	refresh(1,1,graphics->width-2,24);
	refresh(1,25,2,graphics->height-2);
	refresh(graphics->width-3,25,graphics->width-2,graphics->height-2);
	refresh(3,height-3,graphics->width-3,graphics->height-2);
}

void SHEET::window_active(){
	graphics->window_active();
	refresh(1,1,graphics->width-2,24);
	refresh(1,25,2,graphics->height-2);
	refresh(graphics->width-3,25,graphics->width-2,graphics->height-2);
	refresh(3,height-3,graphics->width-3,graphics->height-2);
}

void SHEET::refresh(int bx0,int by0,int bx1,int by1){
	shtctl_refresh(vx0+bx0,vy0+by0,vx0+bx1,vy0+by1);
}

void SHEET::free(){
	shtctl->sheets[height]=NULL;
	if(shtctl->top==height)
		shtctl->top--;
	refresh(0,0,bxsize,bysize);
	flags=0;
	mfree((uintptr_t)graphics->vram,bxsize*bysize*4);
	mfree((uintptr_t)graphics,sizeof(GRAPHICS));
	return;
}
