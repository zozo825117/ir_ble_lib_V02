#include "lptimer.h"
#include "misc.h"


uint8_t  lPTIMER_FLAG  = 0;
/** @defgroup Enter_DeepSleep
  * @{ 
  */
void Enter_DeepSleep(void)
{
	SCB->SCR |= 0x04;   
	__WFI();
}

/** @defgroup Enter_Sleep
  * @{ 
  */
void Enter_Sleep(void)
{
	SCB->SCR &= (~0x04);  
	__WFI();
}


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = _LPTIMER_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}


/** @ test_basetime_timer 
  * @{  
  */
// void  TEST_SETLPTIMERGATE(uint8_t Gate_pin)
// {
// 			if(Gate_pin == 0x00) //gate = TIM11_GATE
// 			{
// 			  GPIO_PinAFConfig(GPIOB,0x04,0x01); //GATE
// 				SYSCTRL_IPTIM_GateConfig(SYSCTRL,(uint32_t)0x0000);
// 			}
// 			else if(Gate_pin == 0x01) //gate = UART0_RXD
// 			{
// 				GPIO_PinAFConfig(GPIOD,0x03,0x05);
// 				SYSCTRL_IPTIM_GateConfig(SYSCTRL,(uint32_t)0x100);
// 			}
// 			else if(Gate_pin == 0x02) //gate = UART1_RXD
// 			{
// 				GPIO_PinAFConfig(GPIOD,0x06,0x05);
// 				SYSCTRL_IPTIM_GateConfig(SYSCTRL,(uint32_t)0x200);
// 			}
// 			else {
// 				if(Gate_pin == 0x03)
// 				{
// 					GPIO_PinAFConfig(GPIOC,0x06,0x05); //LPUART_RXD
// 					SYSCTRL_IPTIM_GateConfig(SYSCTRL,(uint32_t)0x300);
// 				}
// 			}
// }

/** @ test_timer_ini 
  * @{  
  */
void LPTIMER_Config(void)
{
	LPTIMER_InitTypeDef LPTIM_InitStruct;

	/*LPTIM 时钟参数设置*/
  	LPTIM_InitStruct.LPTIMER_Mode     =     _LPTIMER_Mode;
	LPTIM_InitStruct.LPTIMER_CTEN     =     _LPTIMER_CTEN;
	LPTIM_InitStruct.LPTIMER_TCLK     =     _LPTIMER_TCLK;
	// LPTIM_InitStruct.LPTIMER_GATEEN   =     _LPTIMER_GATEEN;
	// LPTIM_InitStruct.LPTIMER_GATEPOLE =     _LPTIMER_GATEPOLE;
	LPTIM_InitStruct.LPTIMER_TCLKCUTEN =    _LPTIMER_TCLKCUTEN;
	LPTIMER_Init(_LPTIMERx, &LPTIM_InitStruct);
	
	/*是否开启TOG功能*/
	// LPTIMER_TogCmd(_LPTIMERx,_LPTIMER_TOG_EN); 
	
	/*设置复用端口*/
	// GPIO_PinAFConfig(_LPTIMER_GPIOx,_LPTIMER_EXT_PIN_SOURCE,_LPTIMER_EXT_AF_VALUE); //EXT
	// GPIO_PinAFConfig(_LPTIMER_GPIOx,_LPTIMER_TOG_PIN_SOURCE,_LPTIMER_TOG_AF_VALUE); //TOG
	// GPIO_PinAFConfig(_LPTIMER_GPIOx,_LPTIMER_TOGN_PIN_SOURCE,_LPTIMER_TOGN_AF_VALUE); //TOGN
	
	
		/*配置复用管脚*/
	// TEST_SETLPTIMERGATE(_LPTIMER_IT_GATEPIN);
	
	NVIC_Configuration();
	
	/*配置LPTIMER功能参数*/
	
	
	LPTIMER_LoadConfig(_LPTIMERx, _LPTIMER_IT_LOADVALUE);
	LPTIMER_BGloadConfig(_LPTIMERx,_LPTIMER_IT_BGLOADVALUE);

	LPTIMER_ITConfig(_LPTIMERx,_LPTIMER_IT_STATUS);
	LPTIMER_Cmd(_LPTIMERx,ENABLE);
	
	
	
	/*进入低功耗模式*/
	// if(_LPTIMER_SLEEP_MODE == 0x01)
	// 	Enter_Sleep();
	// else if(_LPTIMER_SLEEP_MODE == 0x02)
	// 	Enter_DeepSleep();
	// else
	// {;}   //active 状态
		
	return ;
}

uint16_t cnt;
/** @ test_timer_ini 
  * @{  
  */
void _LPTIMER_IRQHandler(void)
{
  	// LPTIMER_ClearITFlag(LPTIMER,LPTIMER_IT_FLAG);
  	_LPTIMERx->ICLR = LPTIMER_IT_FLAG;
	lPTIMER_FLAG = 0x01;
}

