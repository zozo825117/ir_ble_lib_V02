#ifndef __TEST_TIMEBASE_H
#define __TEST_TIMEBASE_H


#include  "cx32l003_timer.h"
#include  "cx32l003_gpio.h"
#include  "cx32l003_syscon.h"
#include "misc.h"


/********************基本定时器TIM参数定义，只限TIM6、7************/
#define            BASIC_TIM                   TIM10
#define            BASIC_TIM_GATE_PIN        0x00  //GATE = TIM10_GATE
//#define            BASIC_TIM_GATE_PIN        0x01  //GATE = UART0_RXD
//#define            BASIC_TIM_GATE_PIN        0x02  //GATE = UART1_RXD
//#define            BASIC_TIM_GATE_PIN        0x02  //GATE = LPUART_RXD


#define            BASIC_TIM_ClKDIV            TIM_Prescale_DIV0
#define            BASIC_TIM_TMROS             TIM_Counter_TMROS_WRAPPING
#define            BASIC_TIM_TMRSZ             TIM_Counter_TMRSZ_32BIT
#define            BASIC_TIM_TMRMS             TIM_Counter_TMRMS_PERIODIC
#define            BASIC_TIM_COUNTERMODE       TIM_CT_TIMER
#define            BASIC_TIM_GATE              TIM_GATE_DISABLE
#define            BASIC_TIM_GATE_POLARITY     TIM_GATE_Polarity_High


#define            BASIC_TIM_TOG_EN            DISABLE
#define            BASIC_TIM_TOG_NE            DISABLE


#define            BASIC_TIM_LOAD_VALUE         0xFFFFA23A


#define            BASIC_TIM_TI_ENABLE         ENABLE

#define            BASIC_TIM_IRQ                TIMER10_IRQn
#define            BASIC_TIM_IRQHandler         TIMER10_IRQHandler

/**************************函数声明********************************/
void BASIC_TIM_init(void);
uint32_t Timer_Get_Time_Stamp(void);
uint8_t Timer_Time_Elapsed(uint32_t time_stamp, uint32_t interval);


#endif


