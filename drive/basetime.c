
// 基本定时器TIMx,x[6,7]定时初始化函数

#include "basetime.h" 

uint32_t time_tick=0;

/**
 * 功能：系统初始化
 * 参数：无
 * 返回：无
 * 备注：初始化定时器0，用作滴答定时器
 */


// 中断优先级配置
static void TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void BASIC_TIM_init(void)
{
    /* timer x*/
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_DeInit(BASIC_TIM);

  TIM_TimeBaseInitStruct.TIM_ClockDivision = BASIC_TIM_ClKDIV;
  TIM_TimeBaseInitStruct.TIM_TMROS  = BASIC_TIM_TMROS;
  TIM_TimeBaseInitStruct.TIM_TMRSZ = BASIC_TIM_TMRSZ;
  TIM_TimeBaseInitStruct.TIM_TMRMS = BASIC_TIM_TMRMS;
  TIM_TimeBaseInitStruct.TIM_CounterMode = BASIC_TIM_COUNTERMODE;
  TIM_TimeBaseInitStruct.TIM_GATE = BASIC_TIM_GATE;
  TIM_TimeBaseInitStruct.TIM_GATE_Polarity = BASIC_TIM_GATE_POLARITY;
  TIM_TimeBaseInit(BASIC_TIM,&TIM_TimeBaseInitStruct);
  
  TIM_SetTimerLoadRegister(BASIC_TIM,BASIC_TIM_LOAD_VALUE);

  TIM_ITConfig(BASIC_TIM,BASIC_TIM_TI_ENABLE);
  TIM_NVIC_Config();

  TIM_Cmd(BASIC_TIM,ENABLE);
}



/**
  * @brief  This function handles TIMER0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void BASIC_TIM_IRQHandler(void)
{
  // TIM_ClearITFlag(BASIC_TIM,TIM_IT_FLAG);
  BASIC_TIM->ICLR = (uint32_t)(0x01&TIM_IT_FLAG);
  // BASIC_TIMFLag = 0x01;
  time_tick++;
}

uint32_t Timer_Get_Time_Stamp(void)
{
    return time_tick;
}

uint8_t Timer_Time_Elapsed(uint32_t time_stamp, uint32_t interval)
{
    uint8_t result = 0;
    if((time_tick - time_stamp) >= interval)
    {
        result = 1;
    }
    else if((time_stamp > time_tick) && ((time_stamp+interval) >= time_tick))
    {
        /* Checking the overflow condition here */
        result = 1;
    }
    return result;
}
