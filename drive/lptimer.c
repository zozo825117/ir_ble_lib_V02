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
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = _LPTIMER_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
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

	/*LPTIM ʱ�Ӳ�������*/
  	LPTIM_InitStruct.LPTIMER_Mode     =     _LPTIMER_Mode;
	LPTIM_InitStruct.LPTIMER_CTEN     =     _LPTIMER_CTEN;
	LPTIM_InitStruct.LPTIMER_TCLK     =     _LPTIMER_TCLK;
	// LPTIM_InitStruct.LPTIMER_GATEEN   =     _LPTIMER_GATEEN;
	// LPTIM_InitStruct.LPTIMER_GATEPOLE =     _LPTIMER_GATEPOLE;
	LPTIM_InitStruct.LPTIMER_TCLKCUTEN =    _LPTIMER_TCLKCUTEN;
	LPTIMER_Init(_LPTIMERx, &LPTIM_InitStruct);
	
	/*�Ƿ���TOG����*/
	// LPTIMER_TogCmd(_LPTIMERx,_LPTIMER_TOG_EN); 
	
	/*���ø��ö˿�*/
	// GPIO_PinAFConfig(_LPTIMER_GPIOx,_LPTIMER_EXT_PIN_SOURCE,_LPTIMER_EXT_AF_VALUE); //EXT
	// GPIO_PinAFConfig(_LPTIMER_GPIOx,_LPTIMER_TOG_PIN_SOURCE,_LPTIMER_TOG_AF_VALUE); //TOG
	// GPIO_PinAFConfig(_LPTIMER_GPIOx,_LPTIMER_TOGN_PIN_SOURCE,_LPTIMER_TOGN_AF_VALUE); //TOGN
	
	
		/*���ø��ùܽ�*/
	// TEST_SETLPTIMERGATE(_LPTIMER_IT_GATEPIN);
	
	NVIC_Configuration();
	
	/*����LPTIMER���ܲ���*/
	
	
	LPTIMER_LoadConfig(_LPTIMERx, _LPTIMER_IT_LOADVALUE);
	LPTIMER_BGloadConfig(_LPTIMERx,_LPTIMER_IT_BGLOADVALUE);

	LPTIMER_ITConfig(_LPTIMERx,_LPTIMER_IT_STATUS);
	LPTIMER_Cmd(_LPTIMERx,ENABLE);
	
	
	
	/*����͹���ģʽ*/
	// if(_LPTIMER_SLEEP_MODE == 0x01)
	// 	Enter_Sleep();
	// else if(_LPTIMER_SLEEP_MODE == 0x02)
	// 	Enter_DeepSleep();
	// else
	// {;}   //active ״̬
		
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

