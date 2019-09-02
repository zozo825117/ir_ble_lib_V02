#ifndef __DELAY_H__
#define __DELAY_H__

/* Includes ------------------------------------------------------------------*/
#include "cx32l003.h"
//#include "cx32l003_conf.h"


// extern uint16_t delay_cycle;
extern uint16_t SystemCoreClockMhz;

void delay_us(uint32_t u32Cnt);
void delay_ms(uint32_t u32Cnt);
void delay_0_5_ms(uint32_t u32Cnt);
void SysTick_Configuration(void);
void CMU_Configuration(void);
void delay_1_us(uint32_t u32Cnt);
#endif



