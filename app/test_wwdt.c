#include "test_wwdt.h"

uint32_t WWDG_FLAG;

// WWDG 中断优先级初始化
static void WWDG_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_WWDG_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/* WWDG 配置函数
 */
void WWDG_Config(void)
{	
	 WWDG_SetPrescaler(WWDG_PRESCALER);
	 WWDG_SetWindowValue(WWDG_WINDOWVALUE);
	
	/*配置WWDG中断使能*/
	 WWDG_CmdTI(WWDG_CMD_TI);
	
	 /*打开WWDG模块功能*/
   WWDG_Cmd(WWDG_CMD_ENABLE);
	
	// 清除提前唤醒中断标志位
	WWDG_ClearFlag();	
	// 配置WWDG中断优先级
	WWDG_NVIC_Config();	

}

// 喂狗
void WWDG_Feed(void)
{
	// 喂狗，刷新递减计数器的值，设置成最大WDG_CNT=0X7F
	WWDG_SetRldCnt( WWDG_CNT );
}


/**
  * @brief  This function handles SPI0COMB_IRQn interrupt request.
  * @param  None
  * @retval None
  */
void DEBUG_WWDG_IRQHandler(void)
{
	WWDG_ClearFlag();
	WWDG_FLAG =0x1;
}





/*********************************************END OF FILE**********************/
