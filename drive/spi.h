/*****************************************************************************
   	Copyright (C), 2018-2019, Hangzhou Greenwhale Science & Technology Group Co.,Ltd
   	File name: DrvSPI.h
   	Author: 
    Version: V0
    Date: 13.Dec.2018  
******************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__
/* Public include files -----------------------------------------------------*/
#include "cx32l003.h"
#include  "cx32l003_gpio.h"

/* Public Macros/Configuration and Data type --------------------------------*/   
// #define  WIRE_SPI_MODE4
#define  WIRE_SPI_MODE3

#define WL1601_CE_Pin   GPIO_Pin_4
#define WL1601_CE_Port  GPIOC

#define WL1601_CSN_Pin GPIO_Pin_3
#define WL1601_CSN_Port GPIOA

#define WL1601_SCK_Pin GPIO_Pin_5
#define WL1601_SCK_Port GPIOC

#define WL1601_MOSI_Pin GPIO_Pin_6
#define WL1601_MOSI_Port GPIOC

#define WL1601_MISO_Pin GPIO_Pin_2
#define WL1601_MISO_Port GPIOD

#define WL1601_PKT_Pin PORT_Pin_3
#define WL1601_PKT_Port GPIOC

// #define WL1601_CE_Output()          WL1601_CE_Port->DIR |= WL1601_CE_Pin
                                    
// #define WL1601_CSN_Output()         WL1601_CSN_Port->DIR |= WL1601_CSN_Pin
                                    
// #define WL1601_SCK_Output()         WL1601_SCK_Port->DIR |= WL1601_SCK_Pin
                                   
#define WL1601_MOSI_Output()        WL1601_MOSI_Port->DIR |= WL1601_MOSI_Pin;\
                                    WL1601_MOSI_Port->PUPD &= ~(GPIO_PuPd_ALL << (6*2));\
                                    WL1601_MOSI_Port->PUPD |= (GPIO_PuPd_UP << (6 *2));
                                                                        
#define WL1601_MOSI_Input()         WL1601_MOSI_Port->DIR &=~ WL1601_MOSI_Pin;\
                                    WL1601_MOSI_Port->PUPD &= ~(GPIO_PuPd_ALL << (6*2));\
                                    WL1601_MOSI_Port->PUPD |= (GPIO_PuPd_NOPULL << (6 *2));
                                                                        
// #define WL1601_MISO_Output()        WL1601_MISO_Port->Pn_SEL0 &=~ WL1601_MISO_Pin;\
//                                     WL1601_MISO_Port->DIR &=~ WL1601_MISO_Pin
                                    
// #define WL1601_MISO_Input()         WL1601_MISO_Port->Pn_SEL0 &=~ WL1601_MISO_Pin;\
//                                     WL1601_MISO_Port->DIR |= WL1601_MISO_Pin
                                                                         
// #define WL1601_MOSI_Read()			(PORT_ReadInputDataBit(WL1601_MOSI_Port,WL1601_MOSI_Pin)&Bit_SET)
// #define WL1601_MISO_Read()			(PORT_ReadInputDataBit(WL1601_MISO_Port,WL1601_MISO_Pin)&Bit_SET)
                                    
// #define WL1601_MOSI_Read()            (WL1601_MOSI_Port->DI & WL1601_MOSI_Pin) != WL1601_MOSI_Pin ? Bit_RESET : Bit_SET
#define WL1601_MOSI_Read()            GPIO_ReadInputDataBit(WL1601_MOSI_Port,WL1601_MOSI_Pin)

#define WL1601_MISO_Read()            (WL1601_MISO_Port->DI & WL1601_MISO_Pin) != WL1601_MISO_Pin ? Bit_RESET : Bit_SET

#define WL1601B_CE_SET()              WL1601_CE_Port->DOSE = WL1601_CE_Pin//PORT_WriteBit(WL1601_CE_Port,WL1601_CE_Pin,Bit_SET)
#define WL1601B_CE_RESET()            WL1601_CE_Port->DOCL = WL1601_CE_Pin//PORT_WriteBit(WL1601_CE_Port,WL1601_CE_Pin,Bit_RESET)
                                      
#define WL1601B_CSN_SET()             WL1601_CSN_Port->DOSE = WL1601_CSN_Pin//PORT_WriteBit(WL1601_CSN_Port,WL1601_CSN_Pin,Bit_SET)
#define WL1601B_CSN_RESET()           WL1601_CSN_Port->DOCL = WL1601_CSN_Pin//PORT_WriteBit(WL1601_CSN_Port,WL1601_CSN_Pin,Bit_RESET)
                                      
#define WL1601_SCK_SET()              WL1601_SCK_Port->DOSE = WL1601_SCK_Pin//PORT_WriteBit(WL1601_SCK_Port,WL1601_SCK_Pin,Bit_SET)
#define WL1601_SCK_RESET()            WL1601_SCK_Port->DOCL = WL1601_SCK_Pin//PORT_WriteBit(WL1601_SCK_Port,WL1601_SCK_Pin,Bit_RESET)
                                      
#define WL1601_MOSI_SET()             WL1601_MOSI_Port->DOSE = WL1601_MOSI_Pin//PORT_WriteBit(WL1601_MOSI_Port,WL1601_MOSI_Pin,Bit_SET)
#define WL1601_MOSI_RESET()           WL1601_MOSI_Port->DOCL = WL1601_MOSI_Pin//PORT_WriteBit(WL1601_MOSI_Port,WL1601_MOSI_Pin,Bit_RESET)
                                      
#define WL1601_MISO_SET()             WL1601_MISO_Port->DOSE = WL1601_MISO_Pin//PORT_WriteBit(WL1601_MISO_Port,WL1601_MISO_Pin,Bit_SET)
#define WL1601_MISO_RESET()           WL1601_MISO_Port->DOCL = WL1601_MISO_Pin//PORT_WriteBit(WL1601_MISO_Port,WL1601_MISO_Pin,Bit_RESET)
          
#ifdef WIRE_SPI_MODE4
#define  DRVSPI_CE_OUTPUT	    WL1601_CE_Output()
#define  DRVWL1601B_CE0       WL1601B_CE_RESET()//PC3 = 0  	 // >>P4.4
#define  DRVWL1601B_CE1       WL1601B_CE_SET()//PC3 = 1  	 // >>P4.4
#else
#define  DRVSPI_CE_OUTPUT  
#define  DRVWL1601B_CE0    
#define  DRVWL1601B_CE1                      
#endif                                    

// #define  DRVSPI_CSN_OUTPUT    WL1601_CSN_Output()
#define  DRVSPI_CSN0          WL1601B_CSN_RESET()
#define  DRVSPI_CSN1          WL1601B_CSN_SET()

// #define  DRVSPI_CLK_OUTPUT 	  WL1601_SCK_Output()
#define  DRVSPI_CLK0       	  WL1601_SCK_RESET()
#define  DRVSPI_CLK1          WL1601_SCK_SET()

#define  DRVSPI_MOSI_INPUT    WL1601_MOSI_Input()
#define  DRVSPI_MOSI_OUTPUT   WL1601_MOSI_Output()
#define  DRVSPI_MOSI0         WL1601_MOSI_RESET()
#define  DRVSPI_MOSI1      	  WL1601_MOSI_SET()
#define  DRVSPI_MOSI          WL1601_MOSI_Read()

// #define  DRVSPI_MISO_INPUT		WL1601_MISO_Input()
#define  DRVSPI_MISO			WL1601_MISO_Read()

/* Enable RF */
#define RF_ENABLE() \
    WL1601B_CE_SET();

/* Disable RF */
#define RF_DISABLE() \
    WL1601B_CE_RESET();


/* Select WL1670 SPI */
#define RF_SPI_SELECT() \
    WL1601B_CSN_SET();

/* Deselect WL1670 SPI */
#define RF_SPI_DESELECT() \
    WL1601B_CSN_RESET(); 


/* Public function declaration ----------------------------------------------*/
void Chip_Reset(void);
void spi_init(void);
void DrvSPI_SendByte2PHASE(uint8_t dat);  //16M主頻->执行一次需要24us
uint8_t DrvSPI_ReceiveByte2PHASE(void);   //16M主頻->执行一次需要24us

uint8_t phy_read_reg(uint8_t reg);
void phy_write_reg(uint8_t reg,uint8_t val);
void phy_write_reg_bit(uint8_t reg,uint8_t bits,uint8_t val);
void phy_read_fifo(uint8_t reg, uint8_t *buf, uint8_t len);
void phy_write_fifo(uint8_t reg, uint8_t *buf, uint8_t len);

/* Public Data declaration --------------------------------------------------*/

#endif




