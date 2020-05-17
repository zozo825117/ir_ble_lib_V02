/*****************************************************************************
* File Name: blecomm.h
*
* Version: 1.0
*
* Description:
* This file contains API for ble communication ble stack.
*
******************************************************************************/
#ifndef __BLECOMM_H__
#define __BLECOMM_H__

#include "spi.h"
#include "cx32l003_timer.h"
#include "cx32l003_syscon.h"
#include "misc.h"
 /** Event callback function prototype to receive events from BLE component */
typedef void (* BLE_CALLBACK_T) (uint32_t eventCode, uint8_t *eventParam);

/** Common error codes received as API result */
typedef enum
{
    /** No Error occurred */
    BLE_ERROR_OK = 0x0000u,
    
    /** At least one of the input parameters is invalid */
    BLE_ERROR_INVALID_PARAMETER,
    
    /** Operation is not permitted */
    BLE_ERROR_INVALID_OPERATION,
  
}BLE_API_RESULT_T;


/** Event handler state machine type */
typedef enum
{
    BLE_STATE_STOPPED,                        /**< BLE is turned off */
    BLE_STATE_INITIALIZING,                   /**< Initializing state */
    BLE_STATE_CONNECTED,                      /**< Peer device is connected */
    BLE_STATE_ADVERTISING,                    /**< Advertising process */
    BLE_STATE_DISCONNECTED                    /**< Essentially idle state */
} BLE_STATE_T;

/** Host stack events.

*/
typedef enum
{
    /** ble stack ready, create at Ble_Init()*/
    BLE_EVT_STACK_ON = 0x01u,
    /** This event is received when there is a timeout and application needs to handle the event.  */
    BLE_EVT_TIMEOUT,

    /** This event indicates that some internal hardware error has occurred. Reset of the hardware may be required. */
    BLE_EVT_HARDWARE_ERROR,

    /** This event is received when there is advertisment start or stop and application needs to handle the event.  
        eventParam[0] : 
            1 start first tigger at Ble_Start() and auto tigger when ble disconnect
            0 stop  clear when ble connect 
    */
    BLE_EVT_GAPP_ADVERTISEMENT_START_STOP,
/** This event is ble receive master scan response event.  
            eventParam : scan respones data from master. 
        */
    BLE_EVT_GAP_SCAN_RESPONSE,
    /** This event is ble connect event.  
            eventParam : link_id[2]  characteristic value of current link, create at connecting. 
        */
    BLE_EVT_GAP_DEVICE_CONNECTED,

    /** This event is ble connect event.  
            eventParam : link_id[2]  characteristic value of current link, create at connecting. 
        */
    BLE_EVT_GAP_DEVICE_DISCONNECTED,
    
    /** Notification data received from server device. Event parameter
       is a pointer to a structure of type BLE_GATTC_HANDLE_VALUE_NTF_PARAM_T. */
    BLE_EVT_GATTC_HANDLE_VALUE_NTF,
    /** 'Prepare Write' Request from client device. Event parameter is a
       pointer to a structure of type BLE_GATTS_PREP_WRITE_REQ_PARAM_T. */
    BLE_EVT_GATTS_PREP_RECEIVE_DATA, 
}BLE_EVENT_T;

/*****************************************************************************
* Function Name: Ble_Init()
******************************************************************************
* Summary:
* This function performs will initialization RF chip and ble stack,
*
* \param TIMER_TypeDef* TIMERx:  ble stack need use one stack timer to working.
* \param IRQn_Type TIMERx_IRQn:  initial timer with interrupt interval is 10ms.
* \param uint32_t CMU_APBPeriph0_TIMERx :  specifies the APB peripheral to gates its clock.
* \param uint32_t clock:   cpu main clock, it recommend mcu clock is HSI_VALUE.
* \param BLE_CALLBACK_T callbackFunc: getting the corresponding events from the BLE component, add application
*                                   code to handle events.
*
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
*
* Note:
*       after function bleState is BLE_STATE_INITIALIZING
*****************************************************************************/
BLE_API_RESULT_T Ble_Init(TIM_TypeDef* TIMERx, IRQn_Type TIMERx_IRQn, uint32_t CMU_APBPeriph0_TIMERx,
    uint32_t clock ,BLE_CALLBACK_T callbackFunc);

/******************************************************************************
* Function Name: Ble_GetMac
*******************************************************************************
* 
* This function gets the Bluetooth device address at BLE Stack's memory(RAM).
* 
* \param uint8_t * deviceMac:   return Bluetooth Device address retrieved from the BLE stack gets stored
*                               to a variable pointed to by this pointer. 
*                               default mac is read in mcu(AR32) chip id
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
*
* Note:
*        default mac is read in mcu(AR32) chip id
* 
******************************************************************************/
BLE_API_RESULT_T Ble_GetMac(uint8_t *deviceMac);

/******************************************************************************
* Function Name: Ble_SetMac
*******************************************************************************
* 
* This function sets the Bluetooth device address into BLE Stack's memory(RAM).
* 
* \param uint8_t * deviceMac:   Bluetooth Device address retrieved from the BLE stack gets stored
*                               to a variable pointed to by this pointer. 
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
*
******************************************************************************/
BLE_API_RESULT_T Ble_SetMac(uint8_t *deviceMac);

/******************************************************************************
* Function Name: Ble_GetName
*******************************************************************************
* 
* This function gets the Bluetooth device advertisement name at BLE Stack's memory(RAM).
* 
* \param uint8_t * deviceName:   return Bluetooth Device advertisement name retrieved from the BLE stack gets stored
*                               to a variable pointed to by this pointer. 
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
*
* Note:
*        default name is "ble comm"
******************************************************************************/
BLE_API_RESULT_T Ble_GetName(uint8_t *deviceName);

/******************************************************************************
* Function Name: Ble_SetName
*******************************************************************************
* 
* This function sets the Bluetooth device advertisement name into BLE Stack's memory(RAM).
* 
* \param uint8_t * deviceName:   Bluetooth Device advertisement name retrieved from the BLE stack gets stored
*                               to a variable pointed to by this pointer. 
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
*
* Note:
*        name length range is [1:15] bytes
******************************************************************************/
BLE_API_RESULT_T Ble_SetName(uint8_t *deviceName);


/******************************************************************************
* Function Name: Ble_Start
*******************************************************************************
* 
* This function start Bluetooth device with Advertising.
* 
* \param 
*  uint16_t time_out
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
* 
* Note:
*       if already ble start will return BLE_ERROR_INVALID_OPERATION
******************************************************************************/
BLE_API_RESULT_T Ble_Start(uint16_t time_out);

/******************************************************************************
* Function Name: Ble_SendData
*******************************************************************************
* 
* This function send data to master.
* 
* \param uint8_t len :  length of send data, range is [1:15]
* \param uint8_t *data :  data pointer 
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
* 
* Note:
*       if bleState isn't BLE_STATE_CONNECTED will return BLE_ERROR_INVALID_OPERATION
*       if data out of range will return CYBLE_ERROR_INVALID_PARAMETER
******************************************************************************/
BLE_API_RESULT_T Ble_SendData(uint8_t len, uint8_t *data);

/******************************************************************************
* Function Name: Ble_pauseSession
*******************************************************************************
* 
* This function is reversed now
*
* \param uint8_t pause :  
*                       1: pause ble session 
*                       0: resume ble session 
*
* \return BLE_STATE_T
*
*
******************************************************************************/
BLE_STATE_T Ble_pauseSession(uint8_t pasue);

/******************************************************************************
* Function Name: Ble_DisConnect
***************************************************************************//**
* 
* This function is device initiative disconnect with master.
* 
* \param None
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER    | On specifying NULL as input parameter.
*   CYBLE_ERROR_INVALID_OPERATION    | Operation is not permitted when device is in connected state.
* 
* Note:
*       if bleState isn't BLE_STATE_CONNECTED will return BLE_ERROR_INVALID_OPERATION
******************************************************************************/
BLE_API_RESULT_T Ble_DisConnect(void);

/******************************************************************************
* Function Name: Ble_Stop
***************************************************************************//**
* 
* This function is stop ble stack and ble stack timer.
* 
* \param None
* 
* \return
*  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or 
*                       failed. Following are the possible error codes.
*
*   Errors codes                     | Description
*   ------------                     | -----------
*   CYBLE_ERROR_OK                   | On successful operation.
* 
* Note:
*       None
******************************************************************************/
BLE_API_RESULT_T Ble_Stop(void);

/******************************************************************************
* Function Name: Ble_GetState
***************************************************************************//**
* 
* This function gets the current bleState.
* 
* \param 
*       None
* 
* \returns: BLE_STATE_T   bleState
*   
* Note:
*       None
******************************************************************************/
BLE_STATE_T Ble_GetState(void);

/******************************************************************************
* Function Name: Ble_ProcessEvents
***************************************************************************//**
*   
*  This function checks the internal task queue in the BLE Stack, and pending
*  operation of the BLE Stack, if any. This needs to be called at least once
*  every interval 't' where:
*   1. 't' is equal to connection interval or scan interval, whichever is
*       smaller, if the device is in GAP Central mode of operation, or
*   2. 't' is equal to connection interval or advertisement interval,
*       whichever is smaller, if the device is in GAP Peripheral mode
*       of operation. 
* 
*  On calling every interval 't', all pending operations of the BLE Stack are
*  processed. This is a blocking function and returns only after processing all
*  pending events of the BLE Stack Care should be taken to prevent this call
*  from any kind of starvation; on starvation, events may be dropped by the
*  stack. All the events generated will be propagated to higher layers of the
*  BLE Stack and to the Application layer only after making a call to this
*  function.
* 
* \return 
*  None
* 
******************************************************************************/
void Ble_ProcessEvents(void);

/******************************************************************************
* Function Name: Ble_BackgroudProcess
***************************************************************************//**
*   
*  This function need place in ble stack timer interrupt 
* 
* \return 
*  None
* 
******************************************************************************/
void Ble_BackgroudProcess(void);

#endif /*__BLECOMM_H__*/
