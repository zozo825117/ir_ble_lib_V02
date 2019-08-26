#include "os.h"
#include <string.h>

#define TASK_MAX 2   //最多可同时运行的任务个数
struct task task_pool[TASK_MAX];//任务池
uint8_t task_tick;  //任务更新标志
/**
 * 功能：任务初始化
 * 参数：无
 * 返回：无
 */
void os_init(void)
{
//	memory_init(); //内存初始化
	memset(task_pool,0,sizeof(struct task)*TASK_MAX);
}

/**
 * 功能：添加任务
 * 参数：time---多久后开始执行，单位ms，fun---要执行的任务
 * 返回：添加成功返回0，否则返回1
 */
uint8_t os_task_add(uint16_t time,taskfun fun)
{
	uint8_t i;
	for(i=0;i<TASK_MAX;i++){
		if((task_pool[i].sta&0x80) == 0){//判断该任务节点是否被使用  sta 的 第7位 为0 表示没有被使用 
			task_pool[i].sta = 0x80;
			task_pool[i].timer = time;
			task_pool[i].fun = fun;
			return 0;
		}
	}
	return 1;
}


/**
 * 功能：更新任务时间
 * 参数：无
 * 返回：0
 */
uint8_t os_task_remark(void)
{
	uint8_t i;
	if(task_tick){
		task_tick = 0;
		for(i=0;i<TASK_MAX;i++){
			if(task_pool[i].timer){
				task_pool[i].timer--;
				if(task_pool[i].timer == 0){
					task_pool[i].sta |=0x01;
				}
			}
		}
	}
	return 0;
}

/**
 * 功能：任务调度
 * 参数：无
 * 返回：0
 */
uint8_t os_task_process(void)
{
	uint8_t i;
	for(i=0;i<TASK_MAX;i++){
		if((task_pool[i].sta&0x01) != 0){
			task_pool[i].sta = 0x00;
			task_pool[i].fun();
			
		}
	}
	return 0;
}



