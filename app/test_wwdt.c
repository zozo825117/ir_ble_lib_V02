#include "test_wwdt.h"

uint32_t WWDG_FLAG;

// WWDG �ж����ȼ���ʼ��
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

/* WWDG ���ú���
 */
void WWDG_Config(void)
{	
	 WWDG_SetPrescaler(WWDG_PRESCALER);
	 WWDG_SetWindowValue(WWDG_WINDOWVALUE);
	
	/*����WWDG�ж�ʹ��*/
	 WWDG_CmdTI(WWDG_CMD_TI);
	
	 /*��WWDGģ�鹦��*/
   WWDG_Cmd(WWDG_CMD_ENABLE);
	
	// �����ǰ�����жϱ�־λ
	WWDG_ClearFlag();	
	// ����WWDG�ж����ȼ�
	WWDG_NVIC_Config();	

}

// ι��
void WWDG_Feed(void)
{
	// ι����ˢ�µݼ���������ֵ�����ó����WDG_CNT=0X7F
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
