#include <stddef.h>
#include <stdint.h>
#include "hanastd.hpp"
#include "task.hpp"
#include "apps.hpp"
#include "apps_api.hpp"
using namespace hanastd;

extern TASKCTRL *taskctl;

void *app_tasklist(char *buffer, uint32_t *cbuffer, char *param) {
	STDOUT out(buffer,cbuffer);
	out.printf(0x97ffd1,"ID");
	out.printf(0xfbff97,"     Name");
	out.printf(0x9bff97," Level");
	out.printf(0xe4acff," Priority");
	out.printf(0x97fffb," Stack Load");
	out.printf(0xaccdff," Status\n");
	for(int i=0;i<MAX_TASKS;i++){
		auto task=&taskctl->tasks0[i];
		if(task->stat!=EMPTY){
			out.printf(0x97ffd1,"%2d",i);
			out.printf(0xfbff97,"%10s",task->name);
			out.printf(0x9bff97,"%6d",task->level);
			out.printf(0xe4acff,"%9d",task->priority);
			if(i==0){
				extern void *kernel_stack_bottom, *kernel_stack_top;
				int kernel_stack_size=&kernel_stack_top-&kernel_stack_bottom;
				int kernel_stack_used=(int)&kernel_stack_top-task->regs.esp;
				out.printf(0x97fffb,"    %2d.%03d%%",kernel_stack_used*100/kernel_stack_size,
						(kernel_stack_used*100000/kernel_stack_size)%1000);
			}else{
				int stack_used=TASK_STACK_SIZE-task->regs.esp+task->stackbottom;
				out.printf(0x97fffb,"    %2d.%03d%%",stack_used*100/TASK_STACK_SIZE,
						(stack_used*100000/TASK_STACK_SIZE)%1000);
			}
			switch(task->stat){
				case RUNNING:
					if(task->level==taskctl->now_lv){
						out.printf(0xaccdff," RUNNING\n");
					} else {
						out.printf(0xaccdff," PREEMPTED\n");
					}
					break;
				case SLEEPING:
					out.printf(0xaccdff," SLEEPING\n");
					break;
				default:
					out.printf(0xaccdff," UNKNOWN\n");
					break;
			}
		}
	}
}
