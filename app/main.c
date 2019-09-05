//#include "test_rtc.h"
//#include "test_calendar.h"
//#include "test_date.h"
#include <string.h>
#include "main.h"
#include "debug_uart.h"
#include "delay.h"
#include "basetime.h"
#include "advtime.h"
#include "blecomm.h"
#include "IRmain.h"

uint8_t bMacAddr[] = {0xCC,0xAA,0x11,0x00,0x21,0x11};


uint8_t device_name[] = "ir ble";
uint8_t wait2_4G = 0;
//uint32_t time_tick=0;
uint32_t timeCloseTimer = 0;
uint32_t timeDisconnectTimer = 0;
uint32_t timeUartReceTimer = 0;
uint32_t runTimeTimer =0;
uint32_t irTimeTimer = 0;
uint32_t send2_4GTimer = 0;

uint8_t uart_rx_buf[20];
uint8_t uart_rx_index;
uint8_t uart_rx_ok;
uint8_t rf_tx_len;
uint8_t test_step = 0;

uint8_t test_send_count = 0;

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void TestPinConfig(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStruct;

		//将USART Rx的GPIO配置为推挽复用模式
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Pin = TEST_GPIO_PORT_PIN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
    GPIO_Init(TEST_GPIO_PORT,&GPIO_InitStruct);	
	


		/* 关闭所有led灯	*/
		GPIO_ResetBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);

}
/**
  * @brief  This function handles TIMER0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIMER11_IRQHandler(void)
{
  // TIM_ClearITFlag(BASIC_TIM,TIM_IT_FLAG);
  TIM11->ICLR = (uint32_t)(0x01&TIM_IT_FLAG);

  Ble_BackgroudProcess();
  // GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
		
}

/*****************************************************************************
* Function Name: app_callback()
******************************************************************************
* Summary:
* This function is a callback that is registered with Ble_Init()
* for getting the corresponding events from the BLE component 
*
* Parameters:
* event - Event from the BLE Component
* eventParam - Pointer to the event parameters 
*
* Return:
* None
* 
* Note:
* None
*****************************************************************************/
void app_callback(uint32_t event, uint8_t * eventParam)
{
	uint8_t i;
	switch(event){
		case BLE_EVT_STACK_ON:
				Debug_Print("BLE_EVT_STACK_ON ble_state= %d\r\n", eventParam[0]);
			break;

		case BLE_EVT_TIMEOUT:
				Debug_Print("BLE_EVT_TIMEOUT link_id= 0x%x%x\r\n", eventParam[0],eventParam[1]);
			break;

		case BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
				Debug_Print("BLE_EVT_GAPP_ADVERTISEMENT_START_STOP state= %d\r\n", eventParam[0]);
			break;

		case BLE_EVT_GAP_DEVICE_CONNECTED:
				Debug_Print("BLE_EVT_GAP_DEVICE_CONNECTED link_id= 0x%x%x\r\n", eventParam[0],eventParam[1]);

				//for ble no deal close
				timeDisconnectTimer = Timer_Get_Time_Stamp();
			break;

		case BLE_EVT_GAP_DEVICE_DISCONNECTED:
				Debug_Print("BLE_EVT_GAP_DEVICE_DISCONNECTED link_id= 0x%x%x\r\n", eventParam[0],eventParam[1]);
			break;

		case BLE_EVT_GATTC_HANDLE_VALUE_NTF:
				Debug_Print("BLE_EVT_GATTC_HANDLE_VALUE_NTF send len= 0x%x\r\n", eventParam[0]);
				Debug_Print(" data=0x");
				for(i=0;i<eventParam[0];i++){
					Debug_Print(" %x", eventParam[i+1]);
				}
				Debug_Print("\r\n");

				//for ble no deal close
				timeDisconnectTimer = Timer_Get_Time_Stamp();
			break;

		case BLE_EVT_GATTS_PREP_RECEIVE_DATA:
				Debug_Print("BLE_EVT_GATTS_PREP_RECEIVE_DATA recv len= 0x%x\r\n", eventParam[0]);
				Debug_Print(" data=0x");
				for(i=0;i<eventParam[0];i++){
					Debug_Print(" %x", eventParam[i+1]);
				}
				Debug_Print("\r\n");

				//for ble no deal close
				timeDisconnectTimer = Timer_Get_Time_Stamp();
				
			break;

	}
}


void testTask(void)
{
//	uint8_t i;
		// *10ms
		if(test_step == 0){
			if(Ble_GetState() == BLE_STATE_INITIALIZING || Ble_GetState() == BLE_STATE_STOPPED){
				if(Ble_Start() == BLE_ERROR_OK){
					Debug_Print("Ble_Start ok\r\n");
				}	
			}
			test_step = 1;

		}else if(test_step == 2){
			if(Timer_Time_Elapsed(timeCloseTimer, 100)){	//100ms
				if(Ble_GetState() != BLE_STATE_STOPPED){
					Debug_Print("now state is %d\r\n",Ble_GetState());
					if(Ble_Stop() == BLE_ERROR_OK){
						Debug_Print("Ble_Stop ok\r\n");
						test_step = 0;
					}
				}
				timeCloseTimer = Timer_Get_Time_Stamp();
			}  
		}

		/*dis connect test*/
		if(Ble_GetState() == BLE_STATE_CONNECTED){
 				// if(Timer_Time_Elapsed(timeDisconnectTimer, 50000)){   //50s
 				// 	Debug_Print(DEBUG_MESSAGE_LEVEL_4,"not operated time exceed %d s\r\n", (50000/100));
 					
					// if(Ble_DisConnect() == BLE_ERROR_OK){

					// 		Debug_Print(DEBUG_MESSAGE_LEVEL_4,"Ble_DisConnect ok\r\n");
					// 		timeCloseTimer = Timer_Get_Time_Stamp();
					// 		test_step = 2;
					// }else{
					// 	Debug_Print(DEBUG_MESSAGE_LEVEL_4,"Ble_DisConnect error state=%d\r\n",Ble_GetState());
					// }
					// timeDisconnectTimer = Timer_Get_Time_Stamp();

 				// }

 				if(uart_rx_ok){
 					if(Ble_SendData(uart_rx_buf[1],&uart_rx_buf[2]) == BLE_ERROR_OK){
							Debug_Print("Ble_SendData ok\r\n");
					}
 					uart_rx_ok = 0;
 				}
		}

		if(Timer_Time_Elapsed(timeUartReceTimer,20)){

				if(uart_rx_index && uart_rx_ok==0){
					uart_rx_index = 0;
				}
		}
		
}

 /**
  * @brief   main
  * @param   无
  * @retval 无
  */
int main( void )
{
	uint8_t mac_buf[6];
	uint8_t name_buf[16];
	
	RCC->AHBCLKEN =  0xffffffff; //flash,crc,gpioa/b/c/d
	RCC->APBCLKEN =  0xffffffff; //uart,i2c,spi,pca,tim,btim, pwm? iwdt,dwdt,rtc,awk,trim
	
	/*CLK TRIMING*/
	RCC->REGLOCK =  0x55aa6699;
	RCC->HIRCCR = 0x5a690EE4;
	RCC->CLKCON  |= 0x5a690005;
	RCC->REGLOCK =  0x55aa6698;

	/*打开外部低频时钟*/
	RCC->REGLOCK = 0x55aa6699;
	/*设置外部低频时钟寄存器*/
	RCC->LXTCR =  0x5a690D6F;
	Delay(10000);
	/*等待外部低频时钟稳定*/
	while(!(RCC->LXTCR&0x40)); 
	/*设置外部低频时钟*/
	RCC->REGLOCK = 0x55aa6698;

	SystemCoreClock    = 24000000;
	// SystemCoreClockMhz = SystemCoreClock/1000000;

	// delay_cycle = (uint16_t)(SystemCoreClock / 1000000 * 81 );
	TestPinConfig();
	
	SysTick_Configuration();
	
		/*配置UART通讯端口*/
	Debug_Print_Init();			
	Delay(1000);
	Debug_Print("hellow SystemCoreClock =%dHz SystemCoreClockMhz=%dMhz\r\n ",
		SystemCoreClock,
		SystemCoreClockMhz);

	// while(1){
	// 	delay_us(1);
	// 	GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
	// }

	BASIC_TIM_init();

	// TEST_TIM_Mode_Config();
	// delay_ms(200000);

	Ble_Init(TIM11,TIMER11_IRQn,0,SystemCoreClock,app_callback);

	if(Ble_GetMac(mac_buf) == BLE_ERROR_OK){
		Debug_Print("default mac = 0x%x%x%x%x%x%x\r\n", mac_buf[0],mac_buf[1],mac_buf[2],mac_buf[3],mac_buf[4],mac_buf[5]);
	}
	else{
		Debug_Print("Ble_GetMac error\r\n");
	}

	if(Ble_SetMac(bMacAddr) == BLE_ERROR_OK){
			Ble_GetMac(mac_buf);
			Debug_Print("new mac = 0x%x%x%x%x%x%x\r\n", mac_buf[0],mac_buf[1],mac_buf[2],mac_buf[3],mac_buf[4],mac_buf[5]);
	}
	else{
		Debug_Print("Ble_SetMac error\r\n");
	}

	memset(name_buf, 0 , sizeof(name_buf));
	if(Ble_GetName(name_buf) == BLE_ERROR_OK){
		Debug_Print("len=%d default name = %s\r\n", strlen((char *)name_buf),name_buf);
	}
	else{
		Debug_Print("Ble_GetName error\r\n");
	}

	if(Ble_SetName(device_name) == BLE_ERROR_OK){
			memset(name_buf, 0 , sizeof(name_buf));
			Ble_GetName(name_buf);
			Debug_Print("len=%d default name =%s\r\n", strlen((char *)name_buf),name_buf);

	}
	else{
		Debug_Print("Ble_SetName error\r\n");
	}

	IRinit();

	IRdata.cmd = IR_CMD_REC_REPLAY;
	
	/*时间获取显示*/
	while(1)
	{
		// delay_0_5_ms(1);
		// GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
		
		// 10ms
		if(Timer_Time_Elapsed(runTimeTimer,10)){
			Ble_ProcessEvents();
			testTask();
			// GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
			runTimeTimer = Timer_Get_Time_Stamp();
		}
		
		if(Timer_Time_Elapsed(irTimeTimer,1000)){
			IRloop(&IRdata);

			if(IRdata.cmd == IR_CMD_SEND){
				if(test_send_count > 10){
					IRdata.cmd = IR_CMD_REC_REPLAY;
					test_send_count = 0;
				} else {
					test_send_count ++;
				}
			}
			// GPIO_ToggleBits(TEST_GPIO_PORT, TEST_GPIO_PORT_PIN);
			irTimeTimer = Timer_Get_Time_Stamp();
		}


		
		
	}
}//main




