#ifndef __TEST_WWDG_H
#define	__TEST_WWDG_H


#include "xm32l003_wwdt.h"
#include "misc.h"

// 窗口计数器值，范围为：0x40~0x7f，一般设置成最大0X7F
#define WWDG_CNT	0X7F

#define WWDG_PRESCALER    WWDG_Prescaler_1
#define WWDG_WINDOWVALUE  0xf0


#define WWDG_CMD_TI         ENABLE
#define WWDG_CMD_ENABLE     ENABLE



#define  DEBUG_WWDG_IRQ                WWDT_IRQn
#define  DEBUG_WWDG_IRQHandler         WWDT_IRQHandler


void WWDG_Config(void);
void WWDG_Feed(void);

#endif /* __BSP_WWDG_H */
