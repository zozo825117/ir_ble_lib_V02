#include "debug_uart.h"
#include "misc.h"
#include "stdio.h"

uint8_t UART_RX_BUF[1];    /*<interrupt recive data buffer*/
uint8_t FlagState = 0;  /*<interrupt recive data flag*/
uint8_t RxCounter = 0;



 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
void Debug_Print_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_InitTypeDef UART_InitStructure;

	//将USART Rx的GPIO配置为推挽复用模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = DEBUG_UART_RX_GPIO_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(DEBUG_UART_RX_GPIO_PORT,&GPIO_InitStruct);

	
	//将USART Tx的GPIO配置为推挽复用模式
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = DEBUG_UART_RX_GPIO_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(DEBUG_UART_TX_GPIO_PORT,&GPIO_InitStruct);
	
	
	  //uart GPIO管脚配置
  GPIO_PinAFConfig(DEBUG_UART_RX_GPIO_PORT,DEBUG_UART_RX_PIN_SOURCE,DEBUG_UART_RX_AF_VALUE); //RX
	GPIO_PinAFConfig(DEBUG_UART_TX_GPIO_PORT,DEBUG_UART_TX_PIN_SOURCE,DEBUG_UART_TX_AF_VALUE); //TX


	// 配置串口的工作参数
	// 配置波特率
	UART_InitStructure.UART_BaudRate = DEBUG_UART_BAUDRATE;
	// 配置 针数据字长
	UART_InitStructure.UART_BaudRateDbaud_Enable = DEBUG_RATEDBAUD_ENABLE;
	// 配置停止位
	UART_InitStructure.UART_BaudRateTimer_Selected = DEBUG_RATETIMER_ENABLE;
	// 配置校验位
	UART_InitStructure.UART_Mode = DEBUG_UART_MODE ;

	// 完成串口的初始化配置
	UART_Init(DEBUG_UARTx,DEBUG_TIMx, &UART_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	

	// 使能串口中断*/
	UART_ITConfig(DEBUG_UARTx,UART_RIEN_EABLE,ENABLE);
	UART_ITConfig(DEBUG_UARTx,UART_TIEN_EABLE,ENABLE);

	
	// 使能串口
	UART_Cmd(DEBUG_UARTx,UART_RXEN_EABLE,ENABLE);	    
}

/*****************  发送一个字节 **********************/
void Uart_SendByte( UART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	UART_SendData(pUSARTx,ch);
	while(FlagState != 0x01);
	FlagState = 0x00;	
}

/****************** 发送8位的数组 ************************/
void Uart_SendArray( UART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Uart_SendByte(pUSARTx,array[i]);	
  
  }

}



/*****************  发送一个16位数 **********************/
void Uart_SendHalfWord( UART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	UART_SendData(pUSARTx,temp_h);	
	//while (UART_GetITStatus(pUSARTx, UART_ISR_TI) == RESET);
	while(FlagState != 0x01);
	FlagState = 0x00;
	
	/* 发送低八位 */
	UART_SendData(pUSARTx,temp_l);	
	//while (UART_GetITStatus(pUSARTx, UART_ISR_TI) == RESET);	
	while(FlagState != 0x01);
	FlagState = 0x00;
}


/*****************  发送字符串 **********************/
void Uart_SendString( UART_TypeDef * pUSARTx, unsigned char *str)
{
	unsigned int k=0;
  do 
  {
      Uart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
}


///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		UART_SendData(DEBUG_UARTx, (uint8_t) ch);
	  while(FlagState != 0x01);
	  FlagState = 0x00;
		return (ch);
}

//重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while(FlagState != 0x01);
	  FlagState = 0x00;
		return (int)UART_ReceiveData(DEBUG_UARTx);
}


//串口中断子程序1
void DEBUG_UART_IRQHandler(void)
{
	if((DEBUG_UARTx->ISR&UART_ISR_TI)==UART_ISR_TI)
	{
	   DEBUG_UARTx->ICR |= UART_ICR_TI;
		 FlagState =  0x01;
	} 
	else if((DEBUG_UARTx->ISR&UART_ISR_RI)==UART_ISR_RI)
	{
		 DEBUG_UARTx->ICR |= UART_ICR_RI;
		 UART_RX_BUF[0]=UART_ReceiveData( DEBUG_UARTx);
		 FlagState =  0x01;
  }	
	else
	{;}
}





