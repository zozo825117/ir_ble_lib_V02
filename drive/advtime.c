#include "advtime.h"
#include "misc.h"
#include "main.h"

uint8_t ucIntCC1Flag = 0;
uint8_t ucIntCC2Flag = 0;
uint8_t ucIntCC3Flag = 0;
uint8_t ucIntCC4Flag = 0;
uint8_t ucIntCOMFlag = 0;
uint8_t ucIntTriggerFlag = 0;
uint8_t ucIntBreakFlag = 0;
uint8_t ucIntServedFlag = 0;


uint32_t ulIntCntTmp = 0;
uint32_t CaptureValue = 0;
uint32_t ulFrequencyCH1 = 0;
uint32_t ulFrequencyCH2 = 0;
uint32_t ulFrequencyCH3 = 0;
uint32_t ulFrequencyCH4 = 0;
uint32_t ulCaptureCC1Old = 0;
uint32_t ulCaptureCC2Old = 0;
uint32_t ulCaptureCC3Old = 0;
uint32_t ulCaptureCC4Old = 0;


static void DELAYCELL(uint32_t count)
{
	uint32_t i,j;
	for(i=0; i<count; i++)
		for(j=0; j<count; j++);
}



static void ADVTIM1_Reset(void)
{
	RCC->REGLOCK = 0x55AA6699;	//reg write protect diable
	RCC->RERIRST |= 1<<10;				//ADVTIM1 reset	
	RCC->REGLOCK = 0x55AA669A;	//reg write protect diable	
}

static void ADVTIM1_ReleaseReset(void)
{
	RCC->REGLOCK = 0x55AA6699;	//reg write protect diable
	RCC->RERIRST &= ~(1<<10);				//ADVTIM1 reset	
	RCC->REGLOCK = 0x55AA669A;	//reg write protect diable	
}


static void GPIO_Reset(void)
{
	RCC->REGLOCK = 0x55AA6699;			//reg write protect diable
	RCC->RERIRST |= 0x0F<<24;				//GPIO reset	
	RCC->REGLOCK = 0x55AA669A;			//reg write protect diable	
}

static void GPIO_ReleaseReset(void)
{
	RCC->REGLOCK = 0x55AA6699;			//reg write protect diable
	RCC->RERIRST &= ~(0x0F<<24);		//GPIO reset	
	RCC->REGLOCK = 0x55AA669A;			//reg write protect diable	
}

/** @defgroup 
  * @{  
  */

static void ADVTIM2_Reset(void)
{
	RCC->REGLOCK = 0x55AA6699;	//reg write protect diable
	RCC->RERIRST |= 1<<11;				//ADVTIM2 reset	
	RCC->REGLOCK = 0x55AA669A;	//reg write protect diable	
}

static void ADVTIM2_ReleaseReset(void)
{
	RCC->REGLOCK = 0x55AA6699;	//reg write protect diable
	RCC->RERIRST &= ~(1<<11);				//ADVTIM2 reset	
	RCC->REGLOCK = 0x55AA669A;	//reg write protect diable	
}




static void DeepSleep(void)
{
	RCC->REGLOCK = 0x55AA6699;			//reg write protect diable	
	NVIC_EnableIRQ(IWDT_IRQn);
	*(unsigned int*)(0xE000ED10) = 0x04;		
	RCC->CLKCON = 0x5A69C006;	// enable LIRC clock
	while((RCC->LIRCCR&0x1000) != 0x1000);
	RCC->CLKSEL = 0x5a690004;
	RCC->CLKCON = 0x5A69C004;		
	
	IWDG->LOCK = 0x55AA6699;
	IWDG->CFG = 0x01;
	IWDG->RLOAD = 0x800;
	IWDG->COMMAND = 0x55;
	__wfi();
	RCC->CLKSEL = 0x5a690004;	//clock change to LIRC
	while(1);
	
}

// 中断优先级配置
static void ADVANCE_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
	  NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQ ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
void TEST_TIM_Mode_Config(void)
{
		ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	

	   /*复位高级TIM*/
   	ADVTIM1_Reset();	
		ADVTIM1_ReleaseReset();	
	  ADVTIM2_Reset();	
		ADVTIM2_ReleaseReset();	
	
	  /*配置高级TIM*/
   	TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;	
    TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;		
    TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;			
    TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;	
		TIM_TimeBaseStructure.TIM_RepetitionCounter = ADVICE_TIM_REPETITONCOUNTER;			
	  ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);	
	
	  /*打开自动装载功能*/
		ADVTIM_ARRPreloadConfig(ADVICE_TIMx, ADVTIM_ARRPRELOAD_EN);
	
	  /*清除中断标志位*/
    ADVTIM_ClearFlag(ADVICE_TIMx, ADVTIM_UPDATA_FLAG);
	
	  /*配置中断*/
		ADVANCE_TIM_NVIC_Config();
    ADVTIM_ITConfig(ADVICE_TIMx,ADVTIM_IT_FLAG,ADVTIM_IT_UPDATE_EN);

//    /*打开高级TIM功能*/
    ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	

}

/** @defgroup 
  * @{  
  */

void TEST_ADVTIM_MULTICHANEEL_Config(void)
{	
	uint16_t PrescalerValue;
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;
	
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;
  TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
  TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
  TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
	
  ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);
	
  /* Prescaler configuration */
  ADVTIM_PrescalerConfig(ADVICE_TIMx, PrescalerValue, TIM_PSCReloadMode_Immediate);
		
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR1_VALUE;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  ADVTIM_OC1Init(ADVTIM1, &TIM_OCInitStructure);

  ADVTIM_OC1PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR2_VALUE;

  ADVTIM_OC2Init(ADVICE_TIMx, &TIM_OCInitStructure);

  ADVTIM_OC2PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR3_VALUE;

  ADVTIM_OC3Init(ADVICE_TIMx, &TIM_OCInitStructure);

  ADVTIM_OC3PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR4_VALUE;

  ADVTIM_OC4Init(ADVICE_TIMx, &TIM_OCInitStructure);

  ADVTIM_OC4PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* TIM IT enable */
  ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
	

		
//  /* ADVTIM1 enable counter */
//  ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	
//	
//	while(ucIntCC1Flag == 0);
//	ucIntCC1Flag = 0;
//	while(ucIntCC2Flag == 0);
//	ucIntCC2Flag = 0;	
//	while(ucIntCC3Flag == 0);
//	ucIntCC3Flag = 0;	
//	while(ucIntCC4Flag == 0);	
//	ucIntCC4Flag = 0;

//	uart_puts(pUARTx,"multichannel compare function interrupt ok\n");

	return ;
}


/** @defgroup 
  * @{  
  */
void TEST_ADVTIM_PWMMODE_Config(void)
{
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC->REGLOCK = 0x55AA6699;
	RCC->IOMUX = 0x5A690000;			//set swdio
	
	GPIOC->AFR = 0x11011000;		//PC3,PC4,PC6,PC7	
	GPIOD->AFR = 0x01110100;		//PD2,PD4,PD6	
		
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
  TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
  TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;;
  TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = ADVICE_TIM_REPETITONCOUNTER;
  ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_TIM_CHANNEL1PULSE;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;                     
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  ADVTIM_OC1Init(ADVICE_TIMx, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = ADVTIM_TIM_CHANNEL2PULSE;
  ADVTIM_OC2Init(ADVICE_TIMx, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = ADVTIM_TIM_CHANNEL3PULSE;
  ADVTIM_OC3Init(ADVICE_TIMx, &TIM_OCInitStructure);
	
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_TIM_CHANNEL4PULSE;
  ADVTIM_OC4Init(ADVICE_TIMx, &TIM_OCInitStructure);

	ADVTIM1->CCMR1 = ADVTIM_TIM_CCMRVALUE;	
 	ADVTIM1->CCMR2 = ADVTIM_TIM_CCMRVALUE;
	
  /* TIM1 counter enable */
  ADVTIM_Cmd(ADVICE_TIMx, ENABLE);
			
  /* TIM1 Main Output Enable */
  ADVTIM_CtrlPWMOutputs(ADVICE_TIMx, ENABLE);	
	
	RCC->REGLOCK = 0x55AA6699;
	RCC->IOMUX = 0x5A690001;			//set swdio
	
  return ;
}



/** @defgroup test_advtim1_capture_mode
  * @{ 
  */
void TEST_ADVTIM_CAPTUREMODE_Config(void)
{	
   	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  ADVTIM_ICInitTypeDef  TIM_ICInitStructure;
		
		/* Time Base configuration */
		TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
		TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
		TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;
		TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = ADVICE_TIM_REPETITONCOUNTER;	
		ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);	
		
		/*配置ICInitStructure结构体*/
		TIM_ICInitStructure.TIM_Channel = ADVTIM_TIM_Channel1;
		TIM_ICInitStructure.TIM_ICPolarity = ADVTIM_TIM_ICPolarity;
		TIM_ICInitStructure.TIM_ICSelection = ADVTIM_TIM_ICSelection;	
		TIM_ICInitStructure.TIM_ICPrescaler = ADVTIM_TIM_ICPrescaler;
		TIM_ICInitStructure.TIM_ICFilter = ADVTIM_TIM_ICFilter;
		

		
		ADVTIM_ICInit(ADVICE_TIMx, &TIM_ICInitStructure);
		
		TIM_ICInitStructure.TIM_Channel = ADVTIM_TIM_Channel2;	
		ADVTIM_ICInit(ADVICE_TIMx, &TIM_ICInitStructure);	

		TIM_ICInitStructure.TIM_Channel = ADVTIM_TIM_Channel3;	
		ADVTIM_ICInit(ADVICE_TIMx, &TIM_ICInitStructure);	
	
		TIM_ICInitStructure.TIM_Channel = ADVTIM_TIM_Channel4;	
		ADVTIM_ICInit(ADVICE_TIMx, &TIM_ICInitStructure);		
	
	  /*配置管脚复用*/
	  GPIO_PinAFConfig(ADVTIM_TIM_CAPTURE_GPIOx,ADVTIM_TIM_CAPTURE_PIN_SOURCE,ADVTIM_TIM_AF_VALUE);
	
		/* Enable the CC1 Interrupt Request */
		ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC1, ENABLE);	
		/* Enable the CC2 Interrupt Request */
		ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC2, ENABLE);	
		/* Enable the CC3 Interrupt Request */
		ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC3, ENABLE);	
		/* Enable the CC4 Interrupt Request */
		ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC4, ENABLE);			
		ADVICE_TIMx->SR = 0x0000;	
		
		ADVANCE_TIM_NVIC_Config();
		
		/* ADVTIM1 enable counter */
		ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	
		
//		while(1)
//		{	
//			delaycell(0xFFF);

//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH1)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel1 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");				
//			ulFrequencyCH1 = 0;
//			
//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH2)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel2 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");				
//			ulFrequencyCH2 = 0;			
//			
//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH3)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel3 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");		
//			ulFrequencyCH3 = 0;		
//			
//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH4)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel4 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");	
//			ulFrequencyCH4 = 0;					
//			
//		}
//			
		return ;
}



/** @defgroup test_advtim1_single_capture_mode
  * @{ 
  */
void TEST_ADVTIM_SINGLECAPTUREMODE_Config(void)
{	
		ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  ADVTIM_ICInitTypeDef  TIM_ICInitStructure;
		
		ADVTIM1_Reset();	
		ADVTIM1_ReleaseReset();			
				
		RCC->REGLOCK = 0x55aa6699;
		RCC->CLKCON |= 0x5a690004;	//LRC enable
		RCC->REGLOCK = 0x55aa669a;	
	
		GPIOA->AFR |= ADVTIM_TIM_Channel;
		if(ADVICE_TIMx == ADVTIM1)
		SYSCTRL->TIM1_CHINMUX |= ADVTIM_TIM_Channel;
	  if(ADVICE_TIMx == ADVTIM2)	
    SYSCTRL->TIM2_CHINMUX |= ADVTIM_TIM_Channel;			
		
		/* Time Base configuration */
		TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
		TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
		TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;
		TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = ADVICE_TIM_REPETITONCOUNTER;	
		ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);	
    /*配置ICInitStructure结构体*/
		TIM_ICInitStructure.TIM_Channel = ADVTIM_TIM_Channel;
		TIM_ICInitStructure.TIM_ICPolarity = ADVTIM_TIM_ICPolarity;
		TIM_ICInitStructure.TIM_ICSelection = ADVTIM_TIM_ICSelection;	
		TIM_ICInitStructure.TIM_ICPrescaler = ADVTIM_TIM_ICPrescaler;
		TIM_ICInitStructure.TIM_ICFilter = ADVTIM_TIM_ICFilter;
		ADVTIM_ICInit(ADVICE_TIMx, &TIM_ICInitStructure);
		
		switch(ADVTIM_TIM_Channel)
		{
			case 0x00:
				/* Enable the CC1 Interrupt Request */
				ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC1, ENABLE);	
				break;
			case 0x04:
				/* Enable the CC2 Interrupt Request */
				ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC2, ENABLE);			
				break;
			case 0x08:
				/* Enable the CC3 Interrupt Request */
				ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC3, ENABLE);				
				break;
			case 0x0C:
				/* Enable the CC4 Interrupt Request */
				ADVTIM_ITConfig(ADVICE_TIMx, TIM_IT_CC4, ENABLE);				
				break;
			default:
				break;
		}	
		
		ucIntCC1Flag = 0;
		ADVICE_TIMx->SR = 0x0000;
		
	  ADVANCE_TIM_NVIC_Config();
		
		/* ADVTIM1 enable counter */
		ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	
		
		while(1)
		{	
			while(ucIntCC1Flag == 0x00);
			ucIntCC1Flag = 0x00;	
	//		k= 0;
	//		for(i=0;i<19;i++)
	//		{
	//			if(advtim_buffer[i+1]>=advtim_buffer[i])
	//			{
	//			data=advtim_buffer[i+1]-advtim_buffer[i];
	//			dec_val[k++] = (char)(data>>8);
	//			dec_val[k++] = (char)(data);
	//			}  
	//			else
	//			{
	//			data=(advtim_buffer[i+1]+0x10000)-advtim_buffer[i];
	//			dec_val[k++] = (char)(data>>8);
	//			dec_val[k++] = (char)(data);

	//			}
	//		}
	//		ByteToHexStr(dec_val,buffer,38);
	//		uart_puts(pUARTx, "\nrev:> ");
	//		uart_puts(pUARTx, buffer);
//		return 0;
//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH2)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel2 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");				
//			ulFrequencyCH2 = 0;			
//			
//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH3)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel3 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");		
//			ulFrequencyCH3 = 0;		
//			
//			for(ulStrCnt=0; ulStrCnt<4; ulStrCnt++)	{
//				HexToStr(&ucRegStr[ulStrCnt*2], (uint8_t *)(&ulFrequencyCH4)+3-ulStrCnt, 1);					
//			}			
//			uart_puts(pUARTx, "Channel4 frequency is 0x");				
//			uart_puts(pUARTx, (char*)ucRegStr);		
//			uart_puts(pUARTx, "\n");	
//			ulFrequencyCH4 = 0;					

//		}
//			
//		return 0;
  
  }
	//return ;
}



/** @defgroup test_advtim1_cascade_mode
  * @{ 
  */    
void TEST_ADVTIM_CASACADEMODE_Config(void)
{	
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;
  TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
  TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
  TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
  ADVTIM_TimeBaseInit(ADVTIM1, &TIM_TimeBaseStructure);
	
  TIM_TimeBaseStructure.TIM_Period =   0x8000;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x8000;	
  ADVTIM_TimeBaseInit(ADVTIM2, &TIM_TimeBaseStructure);
	
  /* Master Configuration in PWM1 Mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 64;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  ADVTIM_OC1Init(ADVTIM1, &TIM_OCInitStructure);
  ADVTIM_OC2Init(ADVTIM1, &TIM_OCInitStructure);
  ADVTIM_OC3Init(ADVTIM1, &TIM_OCInitStructure);	  
	ADVTIM_OC4Init(ADVTIM1, &TIM_OCInitStructure);
	
	ADVTIM1->CCR1 = 0x1000;	
	ADVTIM1->CCR2 = 0x1500;	
	ADVTIM1->CCR3 = 0x1600;	
	ADVTIM1->CCR4 = 0x1700;		
  /* Select the Master Slave Mode */
  ADVTIM_SelectMasterSlaveMode(ADVTIM1, TIM_MasterSlaveMode_Enable);
	
  /* Master Mode selection */
  ADVTIM_SelectOutputTrigger(ADVTIM1, ADVTIM_TIM_TRGOSOURCE);	//EGR bit0 reset ADVTIM2 count	
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_Enable);
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_Update);		
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_OC1);	
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_OC1Ref);	
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_OC2Ref);		
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_OC3Ref);	
//  ADVTIM_SelectOutputTrigger(ADVTIM1, TIM_TRGOSource_OC4Ref);		
	
  /* Slaves Configuration: PWM1 Mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1;
		
  ADVTIM_OC1Init(ADVTIM2, &TIM_OCInitStructure);
	
	
  /* Slave Mode selection: TIM2 */
//  ADVTIM_SelectSlaveMode(ADVTIM2, TIM_SlaveMode_Reset);
//  ADVTIM_SelectSlaveMode(ADVTIM2, TIM_SlaveMode_Gated);	
//  ADVTIM_SelectSlaveMode(ADVTIM2, TIM_SlaveMode_Trigger);		
	ADVTIM_SelectSlaveMode(ADVTIM2, TIM_SlaveMode_External1);	
	
  ADVTIM_SelectInputTrigger(ADVTIM2, TIM_TS_ITR0);
	
  /* TIM enable counter */
  ADVTIM_Cmd(ADVTIM2, ENABLE);
  ADVTIM_Cmd(ADVTIM1, ENABLE);
	
	return ;
}



/** @defgroup test_advtim1_outputcompare
  * @{ 
  */
void TEST_ADVTIM_OUTPUTCOMPARE_Config(void)
{
	
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;
	uint32_t PrescalerValue = PrescalerValue;
	
  /*Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 2000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;
  TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
  TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
  TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
  ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);
	
  /* Output Compare Active Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = ADVTIM_TIM_OCMODE;			//seven mode
  TIM_OCInitStructure.TIM_OutputState = ADVTIM_TIM_OUTPUTSTATE;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR1_VALUE;
  TIM_OCInitStructure.TIM_OCPolarity = ADVTIM_TIM_ICPolarity;
  ADVTIM_OC1Init(ADVICE_TIMx, &TIM_OCInitStructure);
	
  ADVTIM_OC1PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = ADVTIM_TIM_OUTPUTSTATE;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR2_VALUE;

  ADVTIM_OC2Init(ADVICE_TIMx, &TIM_OCInitStructure);

  ADVTIM_OC2PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR3_VALUE;

  ADVTIM_OC3Init(ADVICE_TIMx, &TIM_OCInitStructure);

  ADVTIM_OC3PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);

  /* Output Compare Active Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ADVTIM_CCR4_VALUE;

  ADVTIM_OC4Init(ADVICE_TIMx, &TIM_OCInitStructure);
	
  ADVTIM_OC4PreloadConfig(ADVICE_TIMx, TIM_OCPreload_Disable);
	
  ADVTIM_ARRPreloadConfig(ADVICE_TIMx, ENABLE);
	
  /* ADVTIM1 enable counter */
  ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	

	return ;
}


/** @defgroup test_advtim1_onepulse
  * @{ 
  */
void TEST_ADVTIM_ONEPLUS_Config(void)
{
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;
	ADVTIM_ICInitTypeDef  TIM_ICInitStructure;
	
	ADVTIM1_Reset();	
	ADVTIM1_ReleaseReset();			

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = ADVICE_TIM_PERIOD;
  TIM_TimeBaseStructure.TIM_Prescaler = ADVICE_TIM_PRESCALER;
  TIM_TimeBaseStructure.TIM_ClockDivision = ADVICE_TIM_CLKDIV;
  TIM_TimeBaseStructure.TIM_CounterMode = ADVICE_TIM_COUNTERMODE;
  ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);
	
  /* ADVTIM1 PWM2 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = ADVTIM_TIM_OCMODE;
  TIM_OCInitStructure.TIM_OutputState = ADVTIM_TIM_OUTPUTSTATE;
  TIM_OCInitStructure.TIM_Pulse = 0x8ff;
  TIM_OCInitStructure.TIM_OCPolarity = ADVTIM_TIM_0CPOLARITY;
  ADVTIM_OC1Init(ADVICE_TIMx, &TIM_OCInitStructure);

  /* ADVTIM1 configuration in Input Capture Mode */
  ADVTIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_Channel = ADVTIM_TIM_Channel;
  TIM_ICInitStructure.TIM_ICPolarity = ADVTIM_TIM_ICPolarity;
  TIM_ICInitStructure.TIM_ICSelection = ADVTIM_TIM_ICSelection;
  TIM_ICInitStructure.TIM_ICPrescaler = ADVTIM_TIM_ICPrescaler;
  TIM_ICInitStructure.TIM_ICFilter = ADVTIM_TIM_ICFilter;
  ADVTIM_ICInit(ADVICE_TIMx, &TIM_ICInitStructure);

  /*One Pulse Mode selection */
  ADVTIM_SelectOnePulseMode(ADVICE_TIMx, TIM_OPMode_Single);
	
  /* Input Trigger selection */
  ADVTIM_SelectInputTrigger(ADVICE_TIMx, TIM_TS_TI2FP2);
	
  /* Slave Mode selection: Trigger Mode */
  ADVTIM_SelectSlaveMode(ADVICE_TIMx, TIM_SlaveMode_Trigger);
	  /* TIM1 Main Output Enable */
  ADVTIM_CtrlPWMOutputs(ADVICE_TIMx, ENABLE);	
	//PC6 config as ADVTIM1_CH1 onepulse output
	//PD2 config as ADVTIM1_CH2 trigger input(TI2FP2)	
	GPIOC->AFR = 0x01000000;
	GPIOD->AFR = 0x00000100;
	return ;
}




/** @defgroup test_advtim1_deadtime_break
  * @{ 
  */
void TEST_ADVTIM_DEADTIME_BREAK(void)
{	
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;
	ADVTIM_BDTRInitTypeDef TIM_BDTRInitStructure;

	ADVTIM1_Reset();	
	ADVTIM1_ReleaseReset();			

	TIM_TimeBaseStructure.TIM_Period=  ADVICE_TIM_PERIOD;			
	TIM_TimeBaseStructure.TIM_Prescaler= ADVICE_TIM_PRESCALER;	
	TIM_TimeBaseStructure.TIM_ClockDivision=ADVICE_TIM_CLKDIV;		
	TIM_TimeBaseStructure.TIM_CounterMode=ADVICE_TIM_COUNTERMODE;		
	TIM_TimeBaseStructure.TIM_RepetitionCounter= ADVICE_TIM_REPETITONCOUNTER;	
	ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = ADVTIM_TIM_OCMODE;
	TIM_OCInitStructure.TIM_OutputState = ADVTIM_TIM_OUTPUTSTATE;
	TIM_OCInitStructure.TIM_OutputNState = ADVTIM_TIM_OUTPUTNSTATE; 
	TIM_OCInitStructure.TIM_Pulse = 0x60;
	TIM_OCInitStructure.TIM_OCPolarity = ADVTIM_TIM_0CPOLARITY;
	TIM_OCInitStructure.TIM_OCNPolarity = ADVTIM_TIM_0CNPOLARITY;
	TIM_OCInitStructure.TIM_OCIdleState = ADVTIM_TIM_0CIDLESTATE;
	TIM_OCInitStructure.TIM_OCNIdleState = ADVTIM_TIM_OCNIDLESTATE;
	ADVTIM_OC1Init(ADVICE_TIMx, &TIM_OCInitStructure);
	
//ADVTIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);
	TIM_OCInitStructure.TIM_Pulse = 0x70;	
	ADVTIM_OC2Init(ADVICE_TIMx, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 0x80;	
	ADVTIM_OC3Init(ADVICE_TIMx, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 0x90;	
	ADVTIM_OC4Init(ADVICE_TIMx, &TIM_OCInitStructure);	
	
	/*配置BDTR寄存器*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 50;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  ADVTIM_BDTRConfig(ADVICE_TIMx, &TIM_BDTRInitStructure);
		
	ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	
	ADVTIM_CtrlPWMOutputs(ADVICE_TIMx, ENABLE);
	
	//PC5&PB5 config as BRKIN input function
//	GPIOC->AFR |= 0x00100000;
	//GPIOB->AFR |= 0x00100000;

//	
//	RCC->REGLOCK = 0x55AA6699;
//	RCC->IOMUX = 0x5A690000;			//set swdio as gpio	

	GPIOD->AFR |= 0x01110000;
	GPIOA->AFR |= 0x00001110;
	GPIOC->AFR |= 0x01011000;
	GPIOD->AFR |= 0x00001100;
	
	return ;
}


/** @defgroup test_advtim1_deadtime_break
  * @{ 
  */
void TEST_ADVTIM_DEADTIME_BREAK_DEEPSLEEP_Config(void)
{	
	ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	ADVTIM_OCInitTypeDef  TIM_OCInitStructure;
	ADVTIM_BDTRInitTypeDef TIM_BDTRInitStructure;

	ADVTIM1_Reset();	
	ADVTIM1_ReleaseReset();			
	GPIO_Reset();	
	GPIO_ReleaseReset();		
	
	TIM_TimeBaseStructure.TIM_Period=0x01C0;			
	TIM_TimeBaseStructure.TIM_Prescaler= 0;	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);
	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0x60;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	ADVTIM_OC1Init(ADVICE_TIMx, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 0x70;	
	ADVTIM_OC2Init(ADVICE_TIMx, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 0x80;	
	ADVTIM_OC3Init(ADVICE_TIMx, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 0x90;	
	ADVTIM_OC4Init(ADVICE_TIMx, &TIM_OCInitStructure);	
	

  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 11;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  ADVTIM_BDTRConfig(ADVICE_TIMx, &TIM_BDTRInitStructure);
		
	ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	
	ADVTIM_CtrlPWMOutputs(ADVICE_TIMx, ENABLE);
	
	//PC5&PB5 config as BRKIN input function
//	GPIOC->AFR |= 0x00100000;
	GPIOB->AFR |= 0x00100000;
	
	GPIOD->AFR |= 0x01110000;
	GPIOA->AFR |= 0x00001110;
	GPIOC->AFR |= 0x11011000;
	GPIOD->AFR |= 0x00001100;
		
	DELAYCELL(0xFFF);	
	SYSCTRL->TIM1_CHINMUX |= 1<<21; 	//deep sleep break in enable
	DeepSleep();
	
	return ;
}

/** @defgroup test_advtim1_deadtime_break
  * @{ 
  */
void  TEST_ADVTIM_EVENT_GENERATION_Config(void)
{
		ADVTIM1_Reset();	
		ADVTIM1_ReleaseReset();	
		ADVTIM1->CCER = 0x1111;
	
		ADVICE_TIMx->DIER = 0xFF;	
		NVIC_EnableIRQ(ADVANCE_TIM_IRQ);		
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_Update);	
		while(ucIntServedFlag == 0);
		ucIntServedFlag = 0;
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_CC1);			
		while(ucIntCC1Flag == 0);
		ucIntCC1Flag = 0;
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_CC2);
		while(ADVICE_TIMx == 0);		
		ucIntCC2Flag = 0;
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_CC3);
		while(ucIntCC3Flag == 0);		
		ucIntCC3Flag = 0;
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_CC4);			
		while(ucIntCC4Flag == 0);		
		ucIntCC4Flag = 0;		
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_COM);			
		while(ucIntCOMFlag == 0);		
		ucIntCOMFlag = 0;
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_Trigger);
		while(ucIntTriggerFlag == 0);		
		ucIntTriggerFlag = 0;
		ADVICE_TIMx->BDTR |= 0xB000;
		ADVTIM_GenerateEvent(ADVICE_TIMx, TIM_EventSource_Break);
		while(ucIntBreakFlag == 0);			
		ucIntBreakFlag = 0;					
		return ;
}


/** @defgroup test_advtim1_deadtime_break
  * @{ 
  */
void TEST_ADVTIM_ETR_FUNCTION_Config(void)
{
		ADVTIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		ADVTIM1_Reset();	
		ADVTIM1_ReleaseReset();	
		NVIC_EnableIRQ(ADVANCE_TIM_IRQ);	
	
	  /*配置TIM_TimeBaseStructure结构体*/
		TIM_TimeBaseStructure.TIM_Period=ADVICE_TIM_PERIOD;			
		TIM_TimeBaseStructure.TIM_Prescaler= ADVICE_TIM_PRESCALER;	
		TIM_TimeBaseStructure.TIM_ClockDivision=ADVICE_TIM_CLKDIV;		
		TIM_TimeBaseStructure.TIM_CounterMode=ADVICE_TIM_COUNTERMODE;		
		TIM_TimeBaseStructure.TIM_RepetitionCounter=ADVICE_TIM_REPETITONCOUNTER;	
		ADVTIM_TimeBaseInit(ADVICE_TIMx, &TIM_TimeBaseStructure);
	
	  //external clock enable
		ADVICE_TIMx->SMCR |= 1<<14;	
	  ADVTIM_ITConfig(ADVICE_TIMx,TIM_IT_Update,ENABLE);
		SYSCTRL->TIM1_CHINMUX = ADVTIM_TIM_Channel;
		ADVTIM_Cmd(ADVICE_TIMx, ENABLE);	
		while(ucIntServedFlag == 0);
		ucIntServedFlag = 0;				
		return ;
}


/** @defgroup 中断服务子程序
  * @{ 
  */
void ADVANCE_TIM_IRQHandler(void)
{	
	
	if ( ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_Update) != RESET ) 
	{	
		ulIntCntTmp = ADVTIM1->CNT;	
		ADVTIM_ClearITPendingBit(ADVICE_TIMx , TIM_FLAG_Update);
		ADVTIM_Cmd(ADVICE_TIMx, DISABLE);	
		ADVTIM_ITConfig(ADVICE_TIMx,TIM_IT_Update,DISABLE);
	  ucIntServedFlag = 1;	
	}			

  if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_CC1) != RESET)
  {
			ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_CC1);
			ucIntCC1Flag = 1;
			CaptureValue = ADVTIM_GetCapture1(ADVICE_TIMx);
			ulFrequencyCH1 = 24000000/(CaptureValue - ulCaptureCC1Old);
			ulCaptureCC1Old = CaptureValue;
		
//		  advtim_buffer[cnt++] = ADVTIM_GetCapture1(ADVANCE_TIM);;
//		  ADVTIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC1);
//		  if(cnt == 20)
//			{
//			  cnt = 0;
//        //ADVANCE_TIM->CCER &= ~0x0;
//					/*标志置1*/
//        ucIntCC1Flag = 0x01;
//			}
  }	
  else if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_CC2) != RESET)
  {
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_CC2);
		ucIntCC2Flag = 1;
    CaptureValue = ADVTIM_GetCapture2(ADVICE_TIMx);
		ulFrequencyCH2 = 24000000/(CaptureValue - ulCaptureCC2Old);
		ulCaptureCC2Old = CaptureValue;		
  }
  else if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_CC3) != RESET)
  {
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_CC3);
		ucIntCC3Flag = 1;
    CaptureValue = ADVTIM_GetCapture3(ADVICE_TIMx);
		ulFrequencyCH3 = 24000000/(CaptureValue - ulCaptureCC3Old);
		ulCaptureCC3Old = CaptureValue;
  }
  else if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_CC4) != RESET)
  {
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_CC4);
		ucIntCC4Flag = 1;
    CaptureValue = ADVTIM_GetCapture4(ADVICE_TIMx);
		ulFrequencyCH4 = 24000000/(CaptureValue - ulCaptureCC4Old);
		ulCaptureCC4Old = CaptureValue;				
  }
	
	if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_COM) != RESET)
  {
		ucIntCOMFlag = 1;
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_COM);
  }

	if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_Trigger) != RESET)
  {
		ucIntTriggerFlag = 1;
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_Trigger);
    GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
  }	
	
	if (ADVTIM_GetITStatus(ADVICE_TIMx, TIM_IT_Break) != RESET)
  {
		ucIntBreakFlag = 1;
    ADVTIM_ClearITPendingBit(ADVICE_TIMx, TIM_IT_Break);
  }		
	
	ADVICE_TIMx->SR &= ~0xFFFF;
}
