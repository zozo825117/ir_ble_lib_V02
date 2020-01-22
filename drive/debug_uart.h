#ifndef __CX32LOO3_TEST_UART_H
#define __CX32LOO3_TEST_UART_H


/* Includes --------------------------*/
#include "cx32l003_uart.h"
#include "cx32l003_gpio.h"
#include "cx32l003_rcc.h"



// //// ´®¿Ú0-UART1
// #define  DEBUG_UARTx                   	UART1
// #define  DEBUG_UART_BAUDRATE           	115200

// #define  DEBUG_RATEDBAUD_ENABLE    		ENABLE
// #define  DEBUG_RATETIMER_ENABLE    		DISABLE
// #define  DEBUG_UART_TX_GPIO_PORT       	GPIOD//GPIOC   
// #define  DEBUG_UART_TX_GPIO_PIN        	GPIO_Pin_5//GPIO_Pin_3
// #define  DEBUG_UART_TX_PIN_SOURCE      	GPIO_PinSource5//GPIO_PinSource3
// #define  DEBUG_UART_RX_GPIO_PORT       	GPIOD//GPIOC
// #define  DEBUG_UART_RX_GPIO_PIN        	GPIO_Pin_6//GPIO_Pin_4
// #define  DEBUG_UART_RX_PIN_SOURCE      	GPIO_PinSource6//GPIO_PinSource4

// #define  DEBUG_UART_RX_AF_VALUE         GPIO_AF_UART1_RX_PD6//GPIO_AF_UART1_RX_PC4
// #define  DEBUG_UART_TX_AF_VALUE         GPIO_AF_UART1_TX_PD5//GPIO_AF_UART1_TX_PC3

// #define  DEBUG_UART_MODE               	UART_MODE1
// #define  DEBUG_UART_IRQ                	UART1_IRQn
// #define  DEBUG_UART_IRQHandler         	UART1_IRQHandler

//// ´®¿Ú0-UART0
#define  DEBUG_UARTx                   	UART0
#define  DEBUG_UART_BAUDRATE           	115200

#define  DEBUG_RATEDBAUD_ENABLE    		ENABLE
#define  DEBUG_RATETIMER_ENABLE    		DISABLE
#define  DEBUG_UART_TX_GPIO_PORT       	GPIOB//GPIOC   
#define  DEBUG_UART_TX_GPIO_PIN        	GPIO_Pin_4//GPIO_Pin_3
#define  DEBUG_UART_TX_PIN_SOURCE      	GPIO_PinSource4//GPIO_PinSource3
#define  DEBUG_UART_RX_GPIO_PORT       	GPIOB//GPIOC
#define  DEBUG_UART_RX_GPIO_PIN        	GPIO_Pin_5//GPIO_Pin_4
#define  DEBUG_UART_RX_PIN_SOURCE      	GPIO_PinSource5//GPIO_PinSource4

#define  DEBUG_UART_RX_AF_VALUE         GPIO_AF_UART1_RX_PD6//GPIO_AF_UART1_RX_PC4
#define  DEBUG_UART_TX_AF_VALUE         GPIO_AF_UART1_TX_PD5//GPIO_AF_UART1_TX_PC3

#define  DEBUG_UART_MODE               	UART_MODE1
#define  DEBUG_UART_IRQ                	UART0_IRQn
#define  DEBUG_UART_IRQHandler         	UART0_IRQHandler
#define  DEBUG_TIMx      TIM11
	
#define DEBUG_PRINT

extern uint32_t timeUartReceTimer;
extern uint8_t uart_rx_buf[20];
extern uint8_t uart_rx_index;
extern uint8_t uart_rx_ok;

#ifdef DEBUG_PRINT
void Debug_Print_Init(void);
// void Uart_SendByte( UART_TypeDef * pUARTx, uint8_t ch);
// void Uart_SendString( UART_TypeDef * pUARTx, unsigned char *str);
// void Uart_SendHalfWord( UART_TypeDef * pUARTx, uint16_t ch);
// void Uart_GetString(UART_TypeDef * pUARTx, uint8_t* g_cmd_buf);
// void Uart_SendArray( UART_TypeDef * pUARTx, uint8_t *array, uint16_t num);


#define Debug_Print 						printf

#else
#define Debug_Print_Init()
#define Debug_Print(pszFmt,...)
#endif /* DEBUG_PRINT */



#endif

