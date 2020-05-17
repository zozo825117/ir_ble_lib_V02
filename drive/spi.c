/*****************************************************************************
   	Copyright (C), 2018-2019, Hangzhou Greenwhale Science & Technology Group Co.,Ltd
   	File name: DrvSPI.c
   	Author: 
    Version: V0
    Date: 13.Dec.2018
   	Description: ????SPI
   	
   	Function List: 
   	   	   	   	1-DrvSPI_Init()   	   	   	   	        SPI??IO
   	   	   	   	2-DrvSPI_SendByte2PHASE()   	   	   	????2
   	   	   	   	3-DrvSPI_ReceiveByte2PHASE()   	        ????2
   	   	   	   	4-DrvSPI_SendByte1PHASE()   	   	   	????1
   	   	   	   	5-DrvSPI_ReceiveByte1PHASE()   	        ????1
   	History:  
   	<author>  <time>  <version >  <desc>
******************************************************************************/

/* Private include files ----------------------------------------------------*/
#include "spi.h"
#include "delay.h"
#include "misc.h"

/* Private Macros and Data type ---------------------------------------------*/      	   	   	   	   	   	   	   	   	
/* Private function declaration ----------------------------------------------*/
/* Private Data declaration --------------------------------------------------*/

/*******************************************************************************
   	Function:  	   	   	DrvSPI_Init
   	Description:   	    SPI???
   	Input: 	   	   	   	None
   	Output:	   	   	   	None
   	Return:	   	   	   	None
   	Others:	   	   	   	None
********************************************************************************/
void spi_init(void)
{      
#if defined(WIRE_SPI_MODE4)

//  DRVSPI_MISO_INPUT;
//  DRVSPI_CLK_OUTPUT;
//  DRVSPI_MOSI_OUTPUT;
  GPIO_InitTypeDef GPIO_InitStruct;
#if defined(WIRE_SPI_HARDWARE)
  SPI_InitTypeDef  SPI_InitStruct;
#endif
  // CMU_APBPeriph1ClockCmd(CMU_APBPeriph1_PORT, ENABLE);
  GPIO_InitStruct.GPIO_Pin = WL1601_MISO_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_MISO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = WL1601_SCK_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_SCK_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = WL1601_MOSI_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_MOSI_Port, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = WL1601_CSN_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_CSN_Port, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = WL1601_CE_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_CE_Port, &GPIO_InitStruct);

  // GPIO_PinAFConfig(WL1601_SCK_Port,WL1601_SCK_PinSRC,((uint8_t)0x00)); //Spiclk

#if defined(WIRE_SPI_HARDWARE)
  /*管脚复用*/
  GPIO_PinAFConfig(WL1601_SCK_Port,WL1601_SCK_PinSRC,WL1601_SCK_AF_VALUE); //Spiclk
  // GPIO_PinAFConfig(WL1601_CSN_Port,WL1601_CSN_PinSRC,WL1601_CSN_AF_VALUE); //Spinss 
  GPIO_PinAFConfig(WL1601_MISO_Port,WL1601_MISO_PinSRC,WL1601_MISO_AF_VALUE); //Spimiso
  GPIO_PinAFConfig(WL1601_MOSI_Port,WL1601_MOSI_PinSRC,WL1601_MOSI_AF_VALUE); //Spimosi

  /* 停止信号 WL1601: CS引脚高电平*/
  SPI_SSOutputCmd(SPI,DISABLE);

  /* SPI 模式配置 */
  // WL1601芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
  SPI_InitStruct.SPI_Mode = WL1601_SPI_MODE;  
  SPI_InitStruct.SPI_CPOL = WL1601_SPI_CPOL;  
  SPI_InitStruct.SPI_CPHA = WL1601_SPI_CPHA;  
  SPI_InitStruct.SPI_BaudRatePrescaler = WL1601_SPI_BAUDRATEPRS;  
  SPI_Init(WL1601_SPIx , &SPI_InitStruct);


  SPI_Cmd(WL1601_SPIx,ENABLE);
#else

#if defined(WL1601_SCK_AF_VALUE_PA1)
  GPIO_PinAFConfig(WL1601_SCK_Port,WL1601_SCK_PinSRC,WL1601_SCK_AF_VALUE_PA1); //Spiclk
#endif
  DRVSPI_CLK0;

  DRVSPI_CSN1; //禁止SPI传输
  DRVWL1601B_CE1;
#endif /* defined(WIRE_SPI_HARDWARE) */
#elif defined(WIRE_SPI_MODE3)
  GPIO_InitTypeDef GPIO_InitStruct;
//  DRVSPI_CLK_OUTPUT;
//  DRVSPI_MOSI_OUTPUT;
//  DRVSPI_CSN_OUTPUT;
	// CMU_APBPeriph1ClockCmd(CMU_APBPeriph1_PORT, ENABLE);
  GPIO_InitStruct.GPIO_Pin = WL1601_SCK_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_SCK_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = WL1601_MOSI_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_MOSI_Port, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = WL1601_CSN_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
  GPIO_Init(WL1601_CSN_Port, &GPIO_InitStruct);
  DRVSPI_CLK0;

  // DRVSPI_CE_OUTPUT;
  DRVSPI_CSN1; //禁止SPI传输
  // DRVWL1601B_CE1;
#else
#error "select hardware here\n"  
#endif /* defined(WIRE_SPI_MODE4) */

    // DRVSPI_CE_OUTPUT;
    // DRVSPI_CSN1; //禁止SPI传输
    // DRVWL1601B_CE1;
  	
}

// void delay_1ms(uint16_t u32Cnt){
//   uint32_t counter = u32Cnt<<11;
//   while (counter-- > 0) {
//     __nop();
//   }
// }


/**
 * 函数：phy_read_reg
 * 功能：读RF寄存器
 * 参数：reg----要读取的寄存器地址
 * 返回：读取到的值
 */
uint8_t phy_read_reg(uint8_t reg)
{
   	uint8_t reg_val;	    
   	DRVSPI_CSN0;              	 //使能SPI传输     	   	
   	DrvSPI_SendByte2PHASE(reg | 0x80);    //发送寄存器号
   	reg_val = DrvSPI_ReceiveByte2PHASE();//读取寄存器内容
   	DRVSPI_CSN1;              	 //禁止SPI传输     	   	    
   	return(reg_val);           	 //返回状态值
	
}

/**
 * 函数：phy_write_reg
 * 功能：写RF寄存器
 * 参数：reg----要读取的寄存器地址
 * 			 val---值
 * 返回：无
 */
void phy_write_reg(uint8_t reg,uint8_t val)
{
   	DRVSPI_CSN0;    //使能SPI传输
   	DrvSPI_SendByte2PHASE(reg);     //发送寄存器号 
   	DrvSPI_SendByte2PHASE(val);   //写入寄存器的值
   	DRVSPI_CSN1;    //禁止SPI传输   
}

/**
 * 函数：phy_read_reg_bit
 * 功能：读RF寄存器某一位
 * 参数：reg----要读取的寄存器地址
 * 			 bits----位
 * 返回：0 或 1
 */

//uint8_t phy_read_reg_bit(uint8_t reg,uint8_t bits)
//{
//	data uint8_t val;
//	data uint8_t bit_val = (1 << bits);
//	
//	val = phy_read_reg(reg);
//	
//	return (val & bit_val) ? 1 : 0;
//}
/**
 * 函数：phy_write_reg_bit
 * 功能：写RF寄存器某一位
 * 参数：reg----要读取的寄存器地址
 * 			 bits----位
 * 			 val----值  0 或 1
 * 返回：无
 */
void phy_write_reg_bit(uint8_t reg,uint8_t bits,uint8_t val)
{

	uint8_t tmp;
	uint8_t bit_val = (1 << bits);
	
	tmp = phy_read_reg(reg);
	if (val) {
			tmp |= bit_val;
	} else {
			tmp &= ~bit_val;
	}

	phy_write_reg(reg, tmp);

}


/**
 * 函数：phy_read_fifo
 * 功能：读取RF FIFO中的数据
 * 参数：reg----要读取的寄存器地址
 * 			 buf----读取缓存地址
 * 			 len----读取长度
 * 返回：无
 */
void phy_read_fifo(uint8_t reg, uint8_t *buf, uint8_t len) 
{

   	uint8_t lu8i;

   	DRVSPI_CSN0;	//使能SPI传输
   	DrvSPI_SendByte2PHASE(reg | 0x80);	//发送寄存器号
   	for(lu8i=0; lu8i<len; lu8i++)
    {
        buf[lu8i]=DrvSPI_ReceiveByte2PHASE();
    }
   	DRVSPI_CSN1;	//禁止SPI传输

}

/**
 * 函数：phy_write_fifo
 * 功能：往RF FIFO中写数据
 * 参数：reg----寄存器地址
 * 			 buf----数据缓存地址
 * 			 len----写入长度
 * 返回：无
 */
void phy_write_fifo(uint8_t reg, uint8_t *buf, uint8_t len) 
{

   	uint8_t lu8i;    	
   	
   	DRVSPI_CSN0;        //使能SPI传输
   	DrvSPI_SendByte2PHASE(reg);    	 //发送寄存器号 
   	for(lu8i=0; lu8i<len; lu8i++)
    {
        DrvSPI_SendByte2PHASE(*buf++); //写入数据
    }
   	DRVSPI_CSN1;        //禁止SPI传输

}


/*******************************************************************************
    Function:           
    Description:        
    Input:              None
    Output:             None
    Return:             None
    Others:             None
********************************************************************************/
void Chip_Reset(void)
{
#if defined(WIRE_SPI_MODE4)
  RF_DISABLE();
  RF_SPI_DESELECT();

  /* Reset WL1600 */
  RF_DISABLE();
  delay_us(50);
  
  RF_ENABLE();
  delay_ms(5);
#elif defined(WIRE_SPI_MODE3)
  /**
    SPI_CSN_I long low pulse (>10ms) defined as reset_n 
   */
  DRVSPI_CSN0;    //使能SPI传输
  DrvSPI_SendByte2PHASE(32);     //发送寄存器号 
  DrvSPI_SendByte2PHASE(0x04);   //写入寄存器的值
  DRVSPI_CSN1;    //禁止SPI传输   
  DRVSPI_CSN0;
  delay_ms(20);
  DRVSPI_CSN1;
  delay_ms(10);
  /** 
    设置3线SPI通信 
  */
  DRVSPI_CSN0;    //使能SPI传输
  DrvSPI_SendByte2PHASE(94);     //发送寄存器号 
  DrvSPI_SendByte2PHASE(0x80);   //写入寄存器的值
  DRVSPI_CSN1;    //禁止SPI传输   
#else
#error "select hardware here\n"  
#endif

}

/*******************************************************************************
   	Function:  	   	   	DrvSPI_SendByte2PHASE
   	Description:   	    ????
   	Input: 	   	   	   	None
   	Output:	   	   	   	None
   	Return:	   	   	   	None
   	Others:	   	   	   	None
********************************************************************************/
void DrvSPI_SendByte2PHASE(uint8_t dat)
{  	
   	uint8_t i,SPI_Data;
#if defined(WIRE_SPI_MODE4) 
    #if defined(WIRE_SPI_HARDWARE) 
    /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
    // SPI_SendData(WL1601_SPIx , dat);
    WL1601_SPIx->DATA = dat;
    while((SPI->STAT & SPI_FLAG_SPIF) != SPI_FLAG_SPIF);
    SPI_Data = SPI->DATA;
    #else
   	// for(i=0;i<8;i++){
   	//    	DRVSPI_CLK1;
   	//    	if(dat&0x80)
    //     {
    //       DRVSPI_MOSI1;
    //     }
   	//    	else
    //     {
    //       DRVSPI_MOSI0;
    //     }
   	//    	dat<<=1;
   	//    	DRVSPI_CLK0;
   	// }
   	// DRVSPI_CLK0;
    for(i=0;i<8;i++){
        DRVSPI_CLK0;
        if(dat&0x80)
        {
          DRVSPI_MOSI1;
        }
        else 
        {
          DRVSPI_MOSI0;
        }
        dat<<=1;
        DRVSPI_CLK1;
    }
    DRVSPI_CLK0;
    #endif /* defined(WIRE_SPI_HARDWARE)  */
#elif  defined(WIRE_SPI_MODE3) 
	DRVSPI_MOSI_OUTPUT;
   	for(i=0;i<8;i++){
   	   	DRVSPI_CLK0;
   	   	if(dat&0x80)
        {
          DRVSPI_MOSI1;
        }
   	   	else 
        {
          DRVSPI_MOSI0;
        }
   	   	dat<<=1;
   	   	DRVSPI_CLK1;
   	}
   	DRVSPI_CLK0;
#else
#error "select hardware here\n"  
#endif  /* defined(WIRE_SPI_MODE4)  */
}


/*******************************************************************************
   	Function:  	   	   	DrvSPI_ReceiveByte2PHASE
   	Description:   	    ????
   	Input: 	   	   	   	None
   	Output:	   	   	   	None
   	Return:	   	   	   	None
   	Others:	   	   	   	None
********************************************************************************/
uint8_t DrvSPI_ReceiveByte2PHASE(void)
{
   	uint8_t i,rec=0;

#if defined(WIRE_SPI_MODE4) 
    #if defined(WIRE_SPI_HARDWARE)
    // SPI_SendData(WL1601_SPIx , 0x00);
    WL1601_SPIx->DATA = 0xFF;
    // while(SPIFlagState == 0)
    // {;}
    // SPIFlagState = 0;
    while((SPI->STAT & 0x80) != 0x80);
    rec = SPI->DATA;
    #else
   	for(i=0;i<8;i++){
   	   	DRVSPI_CLK1;
   	   	rec<<=1;
   	   	if(DRVSPI_MISO)
   	   	   	rec|=0x01;
   	   	else 
   	   	   	rec&=0xfe;
   	   	DRVSPI_CLK0;
   	}
   	DRVSPI_CLK0;
    #endif /*   */
#elif defined(WIRE_SPI_MODE3) 
	DRVSPI_MOSI_INPUT;
   	for(i=0;i<8;i++){
   	   	
   	   	rec<<=1;
				DRVSPI_CLK1;
   	   	if(WL1601_MOSI_Read() == Bit_SET)//if(DRVSPI_MOSI)
   	   	   	rec|=0x01;
   	   	else
   	   	   	rec&=0xfe;
   	   	DRVSPI_CLK0;
   	}
   	DRVSPI_CLK0;
#else
#error "select hardware here\n"   
#endif	/* defined(WIRE_SPI_MODE4)  */
   	return rec;
}
