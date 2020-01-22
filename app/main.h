#ifndef  _MAIN_H_
#define  _MAIN_H_



/* Includes ------------------------*/
#include "cx32l003_gpio.h"
#include "cx32l003.h"
#include "cx32l003_conf.h"



/* define ------------------------*/
#define      TEST_GPIO_PORT                      GPIOD
#define      TEST_GPIO_PORT_PIN                  GPIO_Pin_4
#define      TEST_GPIO_PORT_PIN_SOURCE           GPIO_PinSource4

#define TEST_PIN_TOGGLE   	//GPIOD->DO ^= GPIO_Pin_4;

//#define HIRC24M_TIMVALUE   ((*(__IO uint32_t*)0x180000c0)&0x00000fff) /*!< Timing HICR24M Value */

#define HIRC24M_TIMVALUE   ((*(__IO uint16_t*)0x180000c0)) /*!< Timing HICR24M Value */

#endif
