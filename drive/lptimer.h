#ifndef _TEST_LPTIMER_H
#define _TEST_LPTIMER_H

#include "cx32l003_lptimer.h"
#include "cx32l003_syscon.h"
// #include  "cx32l003_gpio.h"

#define   _LPTIMERx        LPTIMER
#define   _LPTIMER_Mode         LPTIMER_MODE1
#define   _LPTIMER_CTEN         LPTIMER_TIMER
#define   _LPTIMER_TCLK         LPTIMER_TCLK_PCLK  //LPTIMER_TCLK_LXT  //LPTIMER_TCLK_LIRC
#define   _LPTIMER_GATEEN       LPTIMER_NGATE
#define   _LPTIMER_GATEPOLE     LPTIMER_GATE_HIGH
#define   _LPTIMER_TCLKCUTEN    LPTIMER_TICK_CUTENABLE
#define   _LPTIMER_TOG_EN       DISABLE

// #define   _LPTIMER_GPIOx                 GPIOD
// #define   _LPTIMER_EXT_PIN_SOURCE        GPIO_PinSource6
// #define   _LPTIMER_TOG_PIN_SOURCE        GPIO_PinSource2
// #define   _LPTIMER_TOGN_PIN_SOURCE       GPIO_PinSource3
// #define   _LPTIMER_EXT_AF_VALUE          GPIO_AF_LPTIMER_EXT_PD6
// #define   _LPTIMER_TOG_AF_VALUE          GPIO_AF_LPTIMER_TOG_PD2
// #define   _LPTIMER_TOGN_AF_VALUE         GPIO_AF_LPTIMER_TOGN_PD3

#define   _LPTIMER_IT_STATUS             ENABLE
#define   _LPTIMER_IT_LOADVALUE          0x01f//0x1ff
#define   _LPTIMER_IT_BGLOADVALUE        0x01f//0x1ff
#define   _LPTIMER_IT_GATEPIN            0x00  //0x00:TIM11_GATE  0x01:UART0_RXD 0x02:UART1_RXD 0x03:LPUART_RXD

#define   _LPTIMER_SLEEP_MODE            0x00    //0x00: active; 0x01 sleepmode  0x02:deepsleepmode


#define   _LPTIMER_IRQ                LPTIMER_IRQn
#define   _LPTIMER_IRQHandler         LPTIMER_IRQHandler

//----------------define function----------------

void LPTIMER_Config(void);
/** @defgroup Enter_DeepSleep
  * @{ 
  */
void Enter_DeepSleep(void);
/** @defgroup Enter_Sleep
  * @{ 
  */
void Enter_Sleep(void);

#endif
