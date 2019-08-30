#ifndef __TEST_ADVANCETIME_H
#define __TEST_ADVANCETIME_H


#include "cx32l003_advtim.h"
#include "cx32l003_gpio.h"
#include "cx32l003_syscon.h"



/********************高级定时器TIM参数定义，只限TIM1、8************/
#define     CCMR_Frozen         		(uint32_t)0x0000
#define     CCMR_ActiveLevel    		(uint32_t)0x1010
#define     CCMR_InActiveLevel  		(uint32_t)0x2020
#define     CCMR_Toggle         		(uint32_t)0x3030
#define     CCMR_ForceInactive  		(uint32_t)0x4040
#define     CCMR_ForceActive    		(uint32_t)0x5050
#define     CCMR_PWMMode1       		(uint32_t)0x6060
#define     CCMR_PWMMode2       		(uint32_t)0x7070

#define     ADVICE_TIMx                         ADVTIM1
#define     ADVICE_TIM_PERIOD                   0x1203
#define		  ADVICE_TIM_PRESCALER                0x0000
#define		  ADVICE_TIM_CLKDIV                   TIM_CKD_DIV1
#define		  ADVICE_TIM_COUNTERMODE              TIM_CounterMode_Up
#define		  ADVICE_TIM_REPETITONCOUNTER         0x00



#define     ADVTIM_ARRPRELOAD_EN                 ENABLE
#define     ADVTIM_UPDATA_FLAG                   TIM_FLAG_Update
#define     ADVTIM_IT_FLAG                       TIM_IT_Update
#define     ADVTIM_IT_UPDATE_EN                  ENABLE



#define	    ADVTIM_CCR1_VALUE       	0xA001
#define	    ADVTIM_CCR2_VALUE       	0x6AAD
#define	    ADVTIM_CCR3_VALUE       	0x3556
#define	    ADVTIM_CCR4_VALUE       	0x1AAA

#define     ADVTIM_TIM_CHANNEL1PULSE         0xE2
#define     ADVTIM_TIM_CHANNEL2PULSE         0xA9
#define     ADVTIM_TIM_CHANNEL3PULSE         0x71
#define     ADVTIM_TIM_CHANNEL4PULSE         0x38

#define     ADVTIM_TIM_OCMODE            TIM_OCMode_PWM1;
#define     ADVTIM_TIM_OUTPUTSTATE       TIM_OutputState_Enable;
#define     ADVTIM_TIM_OUTPUTNSTATE      TIM_OutputNState_Enable;
#define     ADVTIM_TIM_PULSE             ADVTIM_TIM_CHANNEL1PULSE;
#define     ADVTIM_TIM_0CPOLARITY        TIM_OCPolarity_Low;
#define     ADVTIM_TIM_0CNPOLARITY       TIM_OCNPolarity_High;                     
#define     ADVTIM_TIM_0CIDLESTATE       TIM_OCIdleState_Set;
#define     ADVTIM_TIM_OCNIDLESTATE      TIM_OCIdleState_Reset;



#define     ADVTIM_TIM_CCMRVALUE             CCMR_Frozen


#define     ADVTIM_TIM_CAPTURE_GPIOx              GPIOC
#define     ADVTIM_TIM_SINGLECAPTURE_GPIOx        GPIOA
#define     ADVTIM_TIM_CAPTURE_PIN_SOURCE         GPIO_PinSource4
#define     ADVTIM_TIM_AF_VALUE                   GPIO_AF_TIM1_CH4_PC4

#define		  ADVTIM_TIM_Channel1          TIM_Channel_1
#define		  ADVTIM_TIM_Channel2          TIM_Channel_2
#define		  ADVTIM_TIM_Channel3          TIM_Channel_3
#define		  ADVTIM_TIM_Channel4          TIM_Channel_4

#define		  ADVTIM_TIM_ICPolarity       TIM_ICPolarity_Rising
#define		  ADVTIM_TIM_ICSelection      TIM_ICSelection_DirectTI
#define		  ADVTIM_TIM_ICPrescaler      TIM_ICPSC_DIV1
#define		  ADVTIM_TIM_ICFilter         0x0;



#define		  ADVTIM_TIM_Channel          ADVTIM_TIM_Channel1
#define		  ADVTIM_TIM_TRGOSOURCE       TIM_TRGOSource_Reset

#define            ADVANCE_TIM_IRQ               ADVTIM_IRQn
#define            ADVANCE_TIM_IRQHandler        ADVTIM_IRQHandler


/**************************函数声明********************************/

void TEST_TIM_Mode_Config(void);
void TEST_ADVTIM_MULTICHANEEL_Config(void);
void TEST_ADVTIM_PWMMODE_Config(void);
void TEST_ADVTIM_CAPTUREMODE_Config(void);
void TEST_ADVTIM_SINGLECAPTUREMODE_Config(void);
void TEST_ADVTIM_CASACADEMODE_Config(void);
void TEST_ADVTIM_OUTPUTCOMPARE_Config(void);
void TEST_ADVTIM_ONEPLUS_Config(void);
void TEST_ADVTIM_DEADTIME_BREAK(void);
void TEST_ADVTIM_DEADTIME_BREAK_DEEPSLEEP_Config(void);
void TEST_ADVTIM_EVENT_GENERATION_Config(void);
void TEST_ADVTIM_ETR_FUNCTION_Config(void);

#endif	/* __BSP_ADVANCETIME_H */
