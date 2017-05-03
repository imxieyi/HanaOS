#include "asmfunc.hpp"
#include "hanastd.hpp"
#include "heap.hpp"
#include "graphics.hpp"
#include "sheet.hpp"

//图层控制
SHEETCTRL *sheetctrl_init(class MEMMAN *memman,vbe_mode_info_t *vbeinfo){
	int i;
	SHEETCTRL *ctl=(SHEETCTRL *)memman->alloc_4k(sizeof(SHEETCTRL));
	if(ctl==0){
		return 0;
	}
	ctl->setmemman(memman);
	ctl->map=(unsigned char *)memman->alloc_4k(vbeinfo->Xres*vbeinfo->Yres);
	if(ctl->map==0){
		memman->free_4k(int(ctl),sizeof(SHEETCTRL));
		return 0;
	}
	ctl->vram=(uint32_t*)vbeinfo->physbase;
	ctl->xsize=vbeinfo->Xres;
	ctl->ysize=vbeinfo->Yres;
	ctl->fb_stride32=vbeinfo->pitch/sizeof(uint32_t);
	ctl->top=-1;
	for(i=0;i<MAX_SHEETS;i++){
		ctl->sheets0[i].flags=0;//未使用
		ctl->sheets0[i].sctrl=ctl;
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
	int h,old=this->height;
	height=height>sctrl->top+1?sctrl->top+1:height;
	height=height<-1?-1:height;
	this->height=height;
	//重排列sheets[]
	if(old>height){
		if(height>=0){
			for(h=old;h>height;h--){
				sctrl->sheets[h]=sctrl->sheets[h-1];
				sctrl->sheets[h]->height=h;
			}
			sctrl->sheets[height]=this;
			refreshmap(vx0, vy0, vx0 + bxsize, vy0 + bysize, height + 1);
			refreshsub(vx0, vy0, vx0 + bxsize, vy0 + bysize, height + 1, old);
		}else{
			if(sctrl->top>old)
				for(h=old;h<sctrl->top;h++){
					sctrl->sheets[h]=sctrl->sheets[h+1];
					sctrl->sheets[h]->height=h;
				}
			sctrl->top--;
			refreshmap(vx0, vy0, vx0 + bxsize, vy0 + bysize, 0);
			refreshsub(vx0, vy0, vx0 + bxsize, vy0 + bysize, 0, old-1);
		}
	}else if(old<height){
		if(old>=0){
			for(h=old;h<height;h++){
				sctrl->sheets[h]=sctrl->sheets[h+1];
				sctrl->sheets[h]->height=h;
			}
			sctrl->sheets[height]=this;
		}else{
			for(h=sctrl->top;h>=height;h--){
				sctrl->sheets[h+1]=sctrl->sheets[h];
				sctrl->sheets[h+1]->height=h+1;
			}
			sctrl->sheets[height]=this;
			sctrl->top++;
		}
		refreshmap(vx0, vy0, vx0 + bxsize, vy0 + bysize, height);
		refreshsub(vx0, vy0, vx0 + bxsize, vy0 + bysize, height, height);
	}
	return;
}

void SHEET::refresh(int bx0, int by0, int bx1, int by1){
	if (height >= 0)
		refreshsub(vx0 + bx0, vy0 + by0, vx0 + bx1, vy0 + by1, height, height);
	return;
}

void SHEET::refreshsub(int vx0, int vy0, int vx1, int vy1, int h0, int h1){
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	uint32_t *buf, *vram = sctrl->vram;
	uint8_t *map = sctrl->map,sid;
	SHEET *sht;
	vx0=vx0<0?0:vx0;
	vy0=vy0<0?0:vy0;
	vx1=vx1>sctrl->xsize?sctrl->xsize:vx1;
	vy1=vy1>sctrl->ysize?sctrl->ysize:vy1;
	for (h = h0; h <= h1; h++) {
		sht = sctrl->sheets[h];
		buf = sht->graphics->vram;
		sid = sht - sctrl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (map[vy * sctrl->fb_stride32 + vx] == sid) {
					uint32_t oldcolor=vram[vy * sctrl->fb_stride32 + vx];
					uint16_t oldr=(oldcolor>>16)&0xff;
					uint16_t oldg=(oldcolor>>8)&0xff;
					uint16_t oldb=oldcolor&0xff;
					uint32_t bufcolor=buf[by * sctrl->fb_stride32 + bx];
					uint16_t bufr=(bufcolor>>16)&0xff;
					uint16_t bufg=(bufcolor>>8)&0xff;
					uint16_t bufb=bufcolor&0xff;
					uint16_t alpha=(bufcolor>>24)&0xff;
					uint8_t newr=(oldr*(0xff-alpha)+bufr*alpha)/0xff;
					uint8_t newg=(oldg*(0xff-alpha)+bufg*alpha)/0xff;
					uint8_t newb=(oldb*(0xff-alpha)+bufb*alpha)/0xff;
					uint32_t newcolor=VGA_RGBPACK(newr,newg,newb,0xff);
					vram[vy * sctrl->fb_stride32 + vx] = newcolor;
				}
			}
		}
	}
	return;
}

void SHEET::refreshmap(int vx0,int vy0,int vx1,int vy1,int h0){
	int h,bx,by,vx,vy,bx0,by0,bx1,by1;
	uint32_t *buf,sid;
	uint8_t *map=sctrl->map;
	SHEET *sht;
	vx0=vx0<0?0:vx0;
	vy0=vy0<0?0:vy0;
	vx1=vx1>sctrl->xsize?sctrl->xsize:vx1;
	vy1=vy1>sctrl->ysize?sctrl->ysize:vy1;
	for(h=0;h<=sctrl->top;h++){
		sht=sctrl->sheets[h];
		sid=sht-sctrl->sheets0;
		buf=sht->graphics->vram;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				map[vy * sctrl->xsize + vx] = sid;
			}
		}
	}
}

void SHEET::slide(int vx0,int vy0){
	int old_vx0 = this->vx0, old_vy0 = this->vy0;
	this->vx0 = vx0;
	this->vy0 = vy0;
	if (height >= 0) {
		refreshmap(old_vx0, old_vy0, old_vx0 + bxsize, old_vy0 + bysize, 0);
		refreshmap(vx0, vy0, vx0 + bxsize, vy0 + bysize, height);
		refreshsub(old_vx0, old_vy0, old_vx0 + bxsize, old_vy0 + bysize, 0, height - 1);
		refreshsub(vx0, vy0, vx0 + bxsize, vy0 + bysize, height, height);
	}
	return;
}

void SHEET::putstring(int x,int y,int scale,uint32_t f,uint32_t b,char *s){
	int l=hanastd::strlen(s)-1;
	graphics->setcolor(b);
	graphics->boxfill(x,y,x+l*8*scale-1,y+15*scale);
	graphics->setcolor(f);
	graphics->putstr(s,scale,x,y);
	refresh(x,y,x+l*8*scale,y+16*scale);
	return;
}

void SHEET::free(){
	if(height>=0)
		updown(-1);
	flags=0;
	return;
}
