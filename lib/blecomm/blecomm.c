#include<stdio.h>
#include <string.h>
#include "blecomm.h"
#include "ble/inc/comm.h"
#include "ble/inc/netstack_phy.h"
#include "ble/inc/network_conf.h"
#include "os/os.h"

#define   CAL_BASEADDR              (0x180000F0)
#define   CAL0                  		*((volatile unsigned int *)(CAL_BASEADDR+0x00))
#define   CAL1                  		*((volatile unsigned int *)(CAL_BASEADDR+0x02))
#define   CAL2                  		*((volatile unsigned int *)(CAL_BASEADDR+0x04))
#define   CAL3                  		*((volatile unsigned int *)(CAL_BASEADDR+0x06))
#define   CAL4                  		*((volatile unsigned int *)(CAL_BASEADDR+0x08))
#define   CAL5                  		*((volatile unsigned int *)(CAL_BASEADDR+0x0A))
#define   CAL6                  		*((volatile unsigned int *)(CAL_BASEADDR+0x0C))
#define   CAL7                  		*((volatile unsigned int *)(CAL_BASEADDR+0x0E))
	

extern uint8_t task_tick; 
BLE_CALLBACK_T Ble_ApplCallback;
volatile uint8_t cyBle_eventHandlerFlag;
volatile uint8_t eventParam[EVT_PAR_LEN];
volatile uint32_t eventCode;
volatile uint32_t g_time=0;          //全局时间
uint8_t db_mac[6];
uint8_t db_name[16] = {0};
uint8_t send_buf[16] = {0};
uint8_t sending = 0;
volatile uint8_t isInited = 0;
volatile uint8_t started = 0;
volatile BLE_STATE_T bleState;
TIM_TypeDef* bleTIMER;
volatile uint16_t delay_cycle;
volatile uint16_t pairTimeOut;

/*****************************************************************************
* Function Name: Ble_Init()
******************************************************************************
* Summary:
* This function performs the required BLE initialization for various 
* profiles
*
* Parameters:
* TIMER_TypeDef* TIMERx
* IRQn_Type TIMERx_IRQn
* uint32_t int clock
* BLE_CALLBACK_T 

* Return:
* None
*
* Theory:
* Initilizes the BLE component and restores the peer address
*
* Side Effects:
* peerBdAddr and isLinkLossOccured variables are updated
* 
* Note:
* None
*****************************************************************************/
BLE_API_RESULT_T Ble_Init(TIM_TypeDef* TIMERx, IRQn_Type TIMERx_IRQn, uint32_t CMU_APBPeriph0_TIMERx,
    uint32_t clock ,BLE_CALLBACK_T callbackFunc)
{

  /* timer x*/
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
  TIM_DeInit(TIMERx);

  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_Prescale_DIV0;
  TIM_TimeBaseInitStruct.TIM_TMROS  = TIM_Counter_TMROS_WRAPPING;
  TIM_TimeBaseInitStruct.TIM_TMRSZ = TIM_Counter_TMRSZ_32BIT;
  TIM_TimeBaseInitStruct.TIM_TMRMS = TIM_Counter_TMRMS_PERIODIC;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CT_TIMER;
  TIM_TimeBaseInitStruct.TIM_GATE = TIM_GATE_DISABLE;
  TIM_TimeBaseInitStruct.TIM_GATE_Polarity = TIM_GATE_Polarity_High;
  TIM_TimeBaseInit(TIMERx,&TIM_TimeBaseInitStruct);
  
  TIM_SetTimerLoadRegister(TIMERx,0xFFFFA23A);

  TIM_ITConfig(TIMERx,ENABLE);

  // 设置中断组为0
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);   
  // 设置中断来源
  NVIC_InitStructure.NVIC_IRQChannel = TIMERx_IRQn ;  
  // 设置主优先级为 0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
  // 设置抢占优先级为3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIMERx,ENABLE);



  // __enable_irq(); 

  bleTIMER = TIMERx;
  
  // TIMER_Cmd(TIMER_1, ENABLE);

  /**/
  spi_init(); //SPI初始化
  /**/
  CHIP_ID_Get(db_mac);
  Chip_Reset();  
  phy_init(); //物理层初始化
  os_init();  //OS初始化
  mac_init();  //MAC层初始化
  // net_set_mac_addr(bMacAddr);

  net_init();  //网络层初始化

  isInited = 1;

  memcpy(db_name, "ble comm", sizeof("ble comm")); 

  if(clock < (uint32_t)4000000){
    return BLE_ERROR_INVALID_PARAMETER;
  }

  delay_cycle = (uint16_t)(clock / 1000000 * 81 );

  pairTimeOut = 250;

  if ( NULL != callbackFunc ){
    Ble_ApplCallback = callbackFunc;
    bleState = BLE_STATE_INITIALIZING;
    cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
    eventCode = BLE_EVT_STACK_ON;
    memcpy((uint8_t *)eventParam, (uint8_t *)&bleState,sizeof(BLE_EVENT_T));
    return BLE_ERROR_OK;
  } else{
    return BLE_ERROR_INVALID_PARAMETER;
  }
}

void CHIP_ID_Get(uint8_t *chipid)
{
 uint8_t i=0;

 chipid[i++] = (uint8_t)(CAL4 & 0xff);
 chipid[i++] = (uint8_t)((CAL5 >> 8) & 0xff);
 chipid[i++] = (uint8_t)(CAL5 & 0xff);
 chipid[i++] = (uint8_t)((CAL6 >> 8)  & 0xff);
 chipid[i++] = (uint8_t)(CAL6 & 0xff);
 chipid[i++] = (uint8_t)((CAL7 >> 8)  & 0xff);
 // chipid[i++] = (uint8_t)(CAL7 & 0xff);

}

BLE_API_RESULT_T Ble_GetMac(uint8_t *deviceMac)
{
  if( NULL != deviceMac ){

      memcpy(deviceMac, local_info.mac_addr,6);
      return BLE_ERROR_OK;

  }
  return BLE_ERROR_INVALID_PARAMETER;
}

BLE_API_RESULT_T Ble_SetMac(uint8_t *deviceMac)
{
  if( NULL != deviceMac ){
      memcpy(local_info.mac_addr,deviceMac,6);
      return BLE_ERROR_OK;
  }
  return BLE_ERROR_INVALID_PARAMETER;
}

BLE_API_RESULT_T Ble_GetName(uint8_t *deviceName)
{
  if( NULL != deviceName ){
     
      memcpy(deviceName, db_name, strlen((char *)db_name));
      return BLE_ERROR_OK;

    }
    return BLE_ERROR_INVALID_PARAMETER;
}

BLE_API_RESULT_T Ble_SetName(uint8_t *deviceName)
{
  if( NULL != deviceName ){
    if(strlen((char *)deviceName) <=15){
      memset(db_name,0,sizeof(db_name));
      memcpy(db_name,deviceName,strlen((char *)deviceName));
      return BLE_ERROR_OK;
    }
  }
  return BLE_ERROR_INVALID_PARAMETER;
}


BLE_API_RESULT_T Ble_Start(void){
  if(started == 0){
    if(isInited == 0){
      phy_init(); //物理层初始化
      os_init();  //OS初始化
      mac_init();  //MAC层初始化
      // net_set_mac_addr(bMacAddr);

      net_init();  //网络层初始化
      isInited = 1;
    }
    started = 1;
    sending = 0;
    local_info.net_sta = 0;
    TIM_Cmd(bleTIMER, ENABLE);
    __enable_irq(); 

    return BLE_ERROR_OK;
  } else {
    return BLE_ERROR_INVALID_OPERATION;
  } 
}

BLE_API_RESULT_T Ble_Stop()
{
  isInited = 0;
  started = 0;
  TIM_Cmd(bleTIMER, DISABLE);
  bleState = BLE_STATE_STOPPED;
	return BLE_ERROR_OK;
}

BLE_STATE_T Ble_pauseSession(uint8_t pasue)
{
  if(pasue){
    isInited = 0;
    started = 0;
    return bleState;
  }else{
    phy_init(); //物理层初始化
    started = 1;
    isInited = 1;
    return bleState;
  }
}

BLE_STATE_T Ble_GetState()
{
  return bleState;
}

BLE_API_RESULT_T Ble_DisConnect(void)
{
  if(bleState == BLE_STATE_CONNECTED){

      local_info.net_sta = 0;
      bleState = BLE_STATE_DISCONNECTED;
      cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
      eventCode = BLE_EVT_GAP_DEVICE_DISCONNECTED;
      memcpy((uint8_t *)eventParam, local_info.dst,sizeof(local_info.dst));
      return BLE_ERROR_OK;

  }else{
    return BLE_ERROR_INVALID_OPERATION;
  }
}

BLE_API_RESULT_T Ble_SendData(uint8_t len, uint8_t *data){
  if(bleState == BLE_STATE_CONNECTED){

      if(sending == 0){
        if(len != 0 && len <= 15){
          if( NULL != data){
            memset(send_buf,0,sizeof(send_buf));
            send_buf[0] = len;
            memcpy(&send_buf[1], data, len);
            sending = 1;
            return BLE_ERROR_OK;
          }else{
            return BLE_ERROR_INVALID_PARAMETER;
          }

        }else{
          return BLE_ERROR_INVALID_PARAMETER;
        }
        
      }else{
        return BLE_ERROR_INVALID_OPERATION;
      }
  }else{
    return BLE_ERROR_INVALID_OPERATION;
  }
}

void Ble_ProcessEvents(void)
{
  if(started){
    mac_process();
    os_task_remark(); //任务更新
    os_task_process();//任务进程
  }

  if(0u != (cyBle_eventHandlerFlag & CYBLE_CALLBACK )) 
  {
      cyBle_eventHandlerFlag &= (uint8_t)~CYBLE_CALLBACK;
      Ble_ApplCallback((uint32_t)eventCode, (uint8_t *)eventParam);
  }

}

void Ble_BackgroudProcess(void)
{
    g_time++;
    task_tick = 1;

    if(g_time%10 == 0){
        /*tdma*/
        local_info.channel = ++local_info.channel%3;
        if(local_info.pair_time <= pairTimeOut){
          local_info.pair_time++;
        }
    }
}
