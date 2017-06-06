#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "asmfunc.hpp"
#include "task.hpp"
#include "graphics.hpp"
#include "fifo.hpp"
#include "heap.hpp"
#include "apps.hpp"
#include "apps_api.hpp"
#include "dwm.hpp"
#include "rtc.hpp"
using namespace hanastd;

#define NYAN_WIDTH 6
#define NYAN_HEIGHT 4
#define NYAN_SCALE 80
#define OTHER_EDGE 8
#define TOP_EDGE 32
#define FRAME_INTERVAL 100

extern MEMMAN *memman;
extern TIMERCTRL *timerctrl;



void task_binaryclock(void *arg) {
    uint32_t grey = 0x808080, white = 0xffffff, darkgrey = 0x404040;
    SHEET *sht=init_window(2*OTHER_EDGE+NYAN_SCALE*NYAN_WIDTH,
                           OTHER_EDGE+TOP_EDGE+NYAN_SCALE*NYAN_HEIGHT,"Binaryclock");
    auto put_block=[&](char x,char y,uint32_t color){
        if(color==0)
            color=sht->graphics->bgcolor;
        sht->graphics->setcolor(color);
        sht->graphics->boxfill(OTHER_EDGE+x*NYAN_SCALE,TOP_EDGE+y*NYAN_SCALE,
            OTHER_EDGE+(x+1)*NYAN_SCALE-1,TOP_EDGE+(y+1)*NYAN_SCALE-1);
        sht->refresh(OTHER_EDGE+x*NYAN_SCALE,TOP_EDGE+y*NYAN_SCALE,
            OTHER_EDGE+(x+1)*NYAN_SCALE-1,TOP_EDGE+(y+1)*NYAN_SCALE-1);
    };
    int timedata[6][4]={{-1,-1,0,0},{0,0,0,0},{-1,0,0,0},{0,0,0,0},{-1,0,0,0},{0,0,0,0}};
    // refresh function
    auto refresh = [&](){ 
        for(int i=0;i<NYAN_WIDTH;i++)
            for(int j=0;j<NYAN_HEIGHT;j++){
                switch(timedata[i][j]){
                    case -1:
                        put_block(i,j,darkgrey);
                        break;
                    case 0:
                        put_block(i,j,grey);
                        break;
                    case 1:
                        put_block(i,j,white);
                        break;
                }
            }
    };
    auto updatedata = [&](int limit,int digit, int x){
        for(int i=3;i>limit;i--){
            timedata[x][i] = digit % 2;
            digit=digit/2;
        }
    };
    refresh();
    auto task=task_now();
    auto fifo=(FIFO*)memman->alloc_4k(sizeof(FIFO));
    fifo->init(memman,128,task);
    task->fifo=fifo;
    auto timer=timerctrl->alloc()->init(fifo,1);
    timer->set(FRAME_INTERVAL);
    int i = 0;
    for(;;){
        if(!fifo->status()){
            sleepTask();
        }else{
            fifo->get();
            auto time=rtc_time();
            int timedigits[6]={time.hour/10,time.hour %10,time.minute/10,time.minute%10,time.second/10,time.second%10};
            // first digit
            updatedata(1,timedigits[0],0);
            updatedata(-1,timedigits[1],1);
            // second digit
            updatedata(0,timedigits[2],2);
            updatedata(-1,timedigits[3],3);
            // third digit
            updatedata(0,timedigits[4],4);
            updatedata(-1,timedigits[5],5);
            refresh();
               
            timer->set(FRAME_INTERVAL);
        }
    }
}

void *app_binaryclock(char *buffer, uint32_t *cbuffer, char *param) {
    STDOUT out(buffer,cbuffer);
    out.printf(0xffffff,"Hello world! Frank!\n");
    auto task=createTask("binaryclock",&task_binaryclock,NULL);
    //auto time=rtc_time();
    //out.printf(0xffffff,"hour:%d minute: %d second: %d\n",time.hour,time.minute,time.second);
    task_run(task,2,1);
}