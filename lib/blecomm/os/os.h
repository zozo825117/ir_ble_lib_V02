#ifndef __OS_H__
#define __OS_H__

#include "blecomm.h"

typedef void (*taskfun)(void);
struct task{
	int8_t sta;
	uint16_t timer;
	taskfun fun;
};

void os_init(void);
uint8_t os_task_add(uint16_t time,taskfun fun);
uint8_t os_task_remark(void);
uint8_t os_task_process(void);

#endif



