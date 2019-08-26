#include "debug_uart.h"
#include "misc.h"
#include "stdio.h"

uint8_t UART_RX_BUF[1];    /*<interrupt recive data buffer*/
uint8_t FlagState = 0;  /*<interrupt recive data flag*/
uint8_t RxCounter = 0;



 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO ����,������������
  * @param  ��
  * @retval ��
  */
void Debug_Print_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_InitTypeDef UART_InitStructure;

	//��USART Rx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = DEBUG_UART_RX_GPIO_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(DEBUG_UART_RX_GPIO_PORT,&GPIO_InitStruct);

	
	//��USART Tx��GPIO����Ϊ���츴��ģʽ
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = DEBUG_UART_RX_GPIO_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(DEBUG_UART_TX_GPIO_PORT,&GPIO_InitStruct);
	
	
	  //uart GPIO�ܽ�����
  GPIO_PinAFConfig(DEBUG_UART_RX_GPIO_PORT,DEBUG_UART_RX_PIN_SOURCE,DEBUG_UART_RX_AF_VALUE); //RX
	GPIO_PinAFConfig(DEBUG_UART_TX_GPIO_PORT,DEBUG_UART_TX_PIN_SOURCE,DEBUG_UART_TX_AF_VALUE); //TX


	// ���ô��ڵĹ�������
	// ���ò�����
	UART_InitStructure.UART_BaudRate = DEBUG_UART_BAUDRATE;
	// ���� �������ֳ�
	UART_InitStructure.UART_BaudRateDbaud_Enable = DEBUG_RATEDBAUD_ENABLE;
	// ����ֹͣλ
	UART_InitStructure.UART_BaudRateTimer_Selected = DEBUG_RATETIMER_ENABLE;
	// ����У��λ
	UART_InitStructure.UART_Mode = DEBUG_UART_MODE ;

	// ��ɴ��ڵĳ�ʼ������
	UART_Init(DEBUG_UARTx,DEBUG_TIMx, &UART_InitStructure);
	
	// �����ж����ȼ�����
	NVIC_Configuration();
	

	// ʹ�ܴ����ж�*/
	UART_ITConfig(DEBUG_UARTx,UART_RIEN_EABLE,ENABLE);
	UART_ITConfig(DEBUG_UARTx,UART_TIEN_EABLE,ENABLE);

	
	// ʹ�ܴ���
	UART_Cmd(DEBUG_UARTx,UART_RXEN_EABLE,ENABLE);	    
}

/*****************  ����һ���ֽ� **********************/
void Uart_SendByte( UART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	UART_SendData(pUSARTx,ch);
	while(FlagState != 0x01);
	FlagState = 0x00;	
}

/****************** ����8λ������ ************************/
void Uart_SendArray( UART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Uart_SendByte(pUSARTx,array[i]);	
  
  }

}



/*****************  ����һ��16λ�� **********************/
void Uart_SendHalfWord( UART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	UART_SendData(pUSARTx,temp_h);	
	//while (UART_GetITStatus(pUSARTx, UART_ISR_TI) == RESET);
	while(FlagState != 0x01);
	FlagState = 0x00;
	
	/* ���͵Ͱ�λ */
	UART_SendData(pUSARTx,temp_l);	
	//while (UART_GetITStatus(pUSARTx, UART_ISR_TI) == RESET);	
	while(FlagState != 0x01);
	FlagState = 0x00;
}


/*****************  �����ַ��� **********************/
void Uart_SendString( UART_TypeDef * pUSARTx, unsigned char *str)
{
	unsigned int k=0;
  do 
  {
      Uart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
}


///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		UART_SendData(DEBUG_UARTx, (uint8_t) ch);
	  while(FlagState != 0x01);
	  FlagState = 0x00;
		return (ch);
}

//�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while(FlagState != 0x01);
	  FlagState = 0x00;
		return (int)UART_ReceiveData(DEBUG_UARTx);
}


//�����ж��ӳ���1
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





