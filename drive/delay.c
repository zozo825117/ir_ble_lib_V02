#include "delay.h"
#include "misc.h"

//uint16_t delay_cycle;

void SysTick_Configuration(void)
{

    SysTick->VAL = 0;                     //当前值寄存器

//    SysTick_SetReloadValue(0xFFFFFE);		//重装载值
	
	SysTick->LOAD = (0xFFFFFE & (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/));


    // SysTick_CLKSourceConfig(SysTick_ClkSource_ExtRefClk_HSI);//选择时钟源
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择时钟源
    

    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;			//Systick 中断使能位
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;                	//使能 SysTick计数器
}


// void CMU_HSI2HSE(void)
// {
//   // Switch sysclk from IRC16M to HSE(32MHz)
  
//   /* Disable all interrupt */
//   __disable_irq();
  
//   /* At high speed system clock and insert flash read wait cycle */
//   FLASH_ReadWaitCycleCmd(ENABLE);
  
//   // Put on HSE
//   CMU_HSECmd(CMU_CLKStartUpTime3, CMU_HSEDriver7, CMU_HSE_ON);
  
//   // Check HSE is stable
//   while (CMU_WaitForSysClkStartUp(CMU_SysClkSource_HSE) != SET);
  
//   // Switch Systemclock from IRC16M to HSE
//   CMU_SysClkConfig(CMU_SysClkSource_HSE);
  
//   // Close IRC16M
//   CMU_HSIConfig(CMU_IRC16M_Trim_16M, CMU_HSI_OFF);
  
//   /* Enable all interrupt */
//   __enable_irq();   
// }


// void CMU_Configuration(void)
// {
//     CMU_HSI2HSE();
//     CMU_HCLKConfig(CMU_SYSCLK_Div1);
//     CMU_PCLKConfig(CMU_HCLK_Div1);
// }

/**
 * 功能：延时
 */
void delay_us(uint32_t u32Cnt)
{
    uint32_t u32end;
	u32end = 0x1000000 - SystemCoreClock/1000000;
	
    while(u32Cnt-- > 0)
    {
        SysTick->VAL = 0;
   
        while(SysTick->VAL > u32end)
        {
            ;
        }
    }
}
/**
 * 功能：延时
 */
void delay_ms(uint32_t u32Cnt)
{
    uint32_t u32end;
		u32end = 0x1000000 - SystemCoreClock/1000;
    while(u32Cnt-- > 0)
    {
        SysTick->VAL = 0;

        
        while(SysTick->VAL > u32end)
        {
            ;
        }
    }
}

void delay_0_5_ms(uint32_t u32Cnt)
{
    uint32_t u32end;
	
    while(u32Cnt-- > 0)
    {
        SysTick->VAL = 0;

        u32end = 0x1000000 - SystemCoreClock/2000;
        while(SysTick->VAL > u32end)
        {
            ;
        }
    }
}

void delay_1_us(uint32_t u32Cnt)
{
	uint8_t i;

	while(u32Cnt-- > 0)
	{
			for(i = 0; i < 2; i++)
			{
			
			}
	}
}

//void delay_0_5__ms(uint32_t u32Cnt)
//{

//  uint32_t counter = u32Cnt*delay_cycle;
//  while (counter-- > 0) {
//    __nop();
//  }
//}


//void delay_1__us(uint32_t u32Cnt)
//{
//  uint32_t counter = u32Cnt*delay_cycle;
//  while (counter-- > 0) {
//    __nop();
//  }

//}
